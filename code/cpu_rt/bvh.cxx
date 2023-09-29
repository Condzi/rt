namespace rt {
[[nodiscard]] s32
find_longest_axis(AABB const &a) {
  f32 const x = len_sq(a.x);
  f32 const y = len_sq(a.y);
  f32 const z = len_sq(a.z);

  if (x > y && x > z) return 0;
  if (y > x && y > z) return 1;
  return 2;
}

// aka number of spheres per leaf
// @Note: 8 beacuse AVX2.
s32 constexpr static LEAF_WIDTH = 8;
struct Sphere_Pack {
  alignas(32) f32 center_x[LEAF_WIDTH];
  alignas(32) f32 center_y[LEAF_WIDTH];
  alignas(32) f32 center_z[LEAF_WIDTH];
  alignas(32) f32 radius[LEAF_WIDTH];

  Sphere unpacked[LEAF_WIDTH];
};

static std::unordered_map<BVH_Node *, Sphere_Pack> leaf_to_sphere_pack;

s32 constexpr static NUM_BINS      = 12;
s32 constexpr static BIN_NOT_FOUND = -1;

struct BVH_Bin {
  s32  size = 0;
  AABB aabb = {};
};

[[nodiscard]] s32
find_best_axis_using_SAH(Sphere     *spheres,
                         s32         begin,
                         s32         end,
                         AABB const &parent_aabb) {
  f32 best_sah_value = std::numeric_limits<float>::max();
  s32 best_axis      = -1;

  // Iterate over each axis
  for (s32 axis = 0; axis < 3; axis++) {
    // Initialize bins
    BVH_Bin bins[NUM_BINS];

    // Populate the bins
    for (s32 i = begin; i < end; i++) {
      Vec2 const &sphere_axis      = spheres[i].aabb.v[axis];
      Vec2 const &parent_aabb_axis = parent_aabb.v[axis];

      f32 const t = (sphere_axis.min - parent_aabb_axis.min) /
                    (parent_aabb_axis.max - parent_aabb_axis.min);

      s32 const bin_idx = std::min(NUM_BINS - 1, (s32)(t * NUM_BINS));

      bins[bin_idx].size++;
      bins[bin_idx].aabb = make_aabb_from_aabbs(bins[bin_idx].aabb, spheres[i].aabb);
    }

    // Calculate SAH for each bin boundary along this axis
    for (s32 i = 0; i < NUM_BINS - 1; i++) {
      BVH_Bin left_bin = {}, right_bin = {};

      // Accumulate the "left" side: <0, i]
      for (s32 j = 0; j < i; j++) {
        left_bin.aabb = make_aabb_from_aabbs(left_bin.aabb, bins[j].aabb);
        left_bin.size += bins[j].size;
      }

      // Accumulate the "right" side: <i, NUM_BINS]
      for (s32 j = i; j < NUM_BINS; j++) {
        right_bin.aabb = make_aabb_from_aabbs(right_bin.aabb, bins[j].aabb);
        right_bin.size += bins[j].size;
      }

      f32 const left_sah  = left_bin.size * surface_area(left_bin.aabb);
      f32 const right_sah = right_bin.size * surface_area(right_bin.aabb);
      f32 const sah_value = left_sah + right_sah;

      // Update best SAH and corresponding axis
      if (sah_value < best_sah_value) {
        best_sah_value = sah_value;
        best_axis      = axis;
      }
    }
  }

  return best_axis;
}

static s32 best_bin_found     = 0;
static s32 best_bin_not_found = 0;
static s32 empty_slots        = 0;

[[nodiscard]] BVH_Node *
make_BVH(Sphere *spheres, s32 begin, s32 end, AABB const &parent_aabb) {
  // s32 const axis = find_longest_axis(parent_aabb);
  s32 const axis = find_best_axis_using_SAH(spheres, begin, end, parent_aabb);

  auto comparator = [axis](Sphere const &a, Sphere const &b) {
    return a.aabb.v[axis].min < b.aabb.v[axis].min;
  };

  BVH_Node *root = (BVH_Node *)alloc_perm(sizeof(BVH_Node));
  root->aabb     = parent_aabb;

  s32 const object_span = end - begin;
  // leaves CREATION {
  if (object_span <= LEAF_WIDTH) {
    Sphere_Pack leaf_data;
    // Firstly, populate with invalid boxes
    Sphere const &s0 = spheres[begin];
    for (s32 i = 0; i < LEAF_WIDTH; i++) {
      leaf_data.center_x[i] = 0;
      leaf_data.center_y[i] = 0;
      leaf_data.center_z[i] = 0;
      leaf_data.radius[i]   = 0;
      leaf_data.unpacked[i] = s0;
    }

    AABB aabb;
    empty_slots += 8 - object_span;
    for (s32 i = begin; i < end; i++) {
      Sphere const &s_i = spheres[i];

      leaf_data.center_x[i - begin] = s_i.center.x;
      leaf_data.center_y[i - begin] = s_i.center.y;
      leaf_data.center_z[i - begin] = s_i.center.z;
      leaf_data.radius[i - begin]   = s_i.radius;
      leaf_data.unpacked[i - begin] = s_i;

      aabb = make_aabb_from_aabbs(aabb, s_i.aabb);
    }

    // Leaf case (one object) - both children are NULL and the payload is a sphere.
    root->left           = NULL;
    root->right          = NULL;
    root->aabb           = aabb; // is this aabb ok?
    leaf_to_sphere_pack[root] = leaf_data;

    return root;
  }
  // } leaves CREATION

  // Divide the objects into bins along the desired axis {
  BVH_Bin bins[NUM_BINS] = {};
  // Populate the bins
  for (s32 i = begin; i < end; i++) {
    Vec2 const &sphere_axis      = spheres[i].aabb.v[axis];
    Vec2 const &parent_aabb_axis = parent_aabb.v[axis];

    f32 const t = (sphere_axis.min - parent_aabb_axis.min) /
                  (parent_aabb_axis.max - parent_aabb_axis.min);

    s32 const bin_idx = std::min(NUM_BINS - 1, (s32)(t * NUM_BINS));

    bins[bin_idx].size++;
    bins[bin_idx].aabb = make_aabb_from_aabbs(bins[bin_idx].aabb, spheres[i].aabb);
  }

  // Find the best splitting idx
  f32 best_cost = std::numeric_limits<f32>::infinity();
  s32 best_bin  = -1;

  for (s32 i = 0; i < NUM_BINS - 1; i++) {
    BVH_Bin left_bin = {}, right_bin = {};

    // Accumulate the "left" side: <0, i]
    for (s32 j = 0; j < i; j++) {
      left_bin.aabb = make_aabb_from_aabbs(left_bin.aabb, bins[j].aabb);
      left_bin.size += bins[j].size;
    }

    // Accumulate the "right" side: <i, NUM_BINS]
    for (s32 j = i; j < NUM_BINS; j++) {
      right_bin.aabb = make_aabb_from_aabbs(right_bin.aabb, bins[j].aabb);
      right_bin.size += bins[j].size;
    }

    // Calculate the cost
    f32 const left_ratio  = left_bin.size * surface_area(left_bin.aabb);
    f32 const right_ratio = right_bin.size * surface_area(right_bin.aabb);
    f32 const cost = (1.0f + (left_ratio + right_ratio)) / surface_area(parent_aabb);

    if (cost < best_cost) {
      best_cost = cost;
      best_bin  = i;
    }
  }
  // } Binning

  s32 mid = -1;

  // Try to do a bin split
  if (best_bin != BIN_NOT_FOUND) {
    Vec2 const &parent_aabb_axis = parent_aabb.v[axis];
    f32 const   split_value =
        parent_aabb_axis.min +
        (best_bin + 1) * (parent_aabb_axis.max - parent_aabb_axis.min) / NUM_BINS;

    auto const binning_comparator = [axis, split_value](Sphere const &s) {
      return s.aabb.v[axis].min < split_value;
    };

    mid = (s32)(std::partition(spheres + begin, spheres + end, binning_comparator) -
                spheres);

    // All spheres fell into one bin - it's not a valid split!
    if (mid == begin || mid == end) {
      best_bin = BIN_NOT_FOUND;
    } else {
      best_bin_found++;
    }
  }

  // Fallback to median split if bin split is not correct
  if (best_bin == BIN_NOT_FOUND) {
    best_bin_not_found++;
    std::sort(spheres + begin, spheres + end, comparator);

    mid = begin + object_span / 2;
  }
  // Precalculate AABBs
  AABB left_aabb, right_aabb;
  for (s32 i = begin; i < mid; i++) {
    left_aabb = make_aabb_from_aabbs(left_aabb, spheres[i].aabb);
  }

  for (s32 i = mid; i < end; i++) {
    right_aabb = make_aabb_from_aabbs(right_aabb, spheres[i].aabb);
  }

  root->left  = make_BVH(spheres, begin, mid, left_aabb);
  root->right = make_BVH(spheres, mid, end, right_aabb);

  /*
    logf("best_bin_found=%d \t best_bin_not_found=%d\n",
         best_bin_found,
         best_bin_not_found);
  */

  // logf("empty_slots=%d\n", empty_slots);

  return root;
}

thread_local std::vector<BVH_Node *> candidates;
void
hit_BVH_internal(BVH_Node *root, Vec3 const &ray_origin, Vec3 const &ray_inv_dir) {
  if (!ray_vs_aabb(
          ray_origin, ray_inv_dir, {.min = 0.001f, .max = FLT_MAX}, root->aabb)) {
    return;
  }

  // Potential hit
  if (root->left == NULL && root->right == NULL) {
    candidates.push_back(root);
    return;
  }

  hit_BVH_internal(root->left, ray_origin, ray_inv_dir);
  hit_BVH_internal(root->right, ray_origin, ray_inv_dir);
}

[[nodiscard]] u32
discriminant_check(Ray const &r, Sphere_Pack const &s_pack) {
  // Load packed sphere data into AVX2 registers
  __m256 center_x = _mm256_loadu_ps(s_pack.center_x);
  __m256 center_y = _mm256_loadu_ps(s_pack.center_y);
  __m256 center_z = _mm256_loadu_ps(s_pack.center_z);
  __m256 radius   = _mm256_loadu_ps(s_pack.radius);

  // Calculate oc = r.origin - s.center for each packed sphere
  __m256 oc_x = _mm256_sub_ps(_mm256_set1_ps(r.origin.x), center_x);
  __m256 oc_y = _mm256_sub_ps(_mm256_set1_ps(r.origin.y), center_y);
  __m256 oc_z = _mm256_sub_ps(_mm256_set1_ps(r.origin.z), center_z);

  // Calculate a, half_b, and c for each packed sphere
  __m256 a =
      _mm256_set1_ps(r.direction.x * r.direction.x + r.direction.y * r.direction.y +
                     r.direction.z * r.direction.z);
  __m256 half_b =
      _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(oc_x, _mm256_set1_ps(r.direction.x)),
                                  _mm256_mul_ps(oc_y, _mm256_set1_ps(r.direction.y))),
                    _mm256_mul_ps(oc_z, _mm256_set1_ps(r.direction.z)));
  __m256 c = _mm256_sub_ps(_mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(oc_x, oc_x),
                                                       _mm256_mul_ps(oc_y, oc_y)),
                                         _mm256_mul_ps(oc_z, oc_z)),
                           _mm256_mul_ps(radius, radius));

  // Calculate the discriminant for each packed sphere
  __m256 discriminant =
      _mm256_sub_ps(_mm256_mul_ps(half_b, half_b), _mm256_mul_ps(a, c));

  // Check if any discriminant is >= 0
  __m256 mask = _mm256_cmp_ps(discriminant, _mm256_setzero_ps(), _CMP_GE_OQ);

  u32 mask_val = (u32)_mm256_movemask_ps(mask);
  return mask_val;
}

[[nodiscard]] bool
hit_BVH(BVH_Node *root, Ray const &ray, Vec2 t, Hit_Info &hi) {
  candidates.reserve(128);
  candidates.clear();

  hit_BVH_internal(root, ray.origin, ray.direction_inv);

  bool any_hit = false;

  for (BVH_Node *node : candidates) {
    auto const &pack = leaf_to_sphere_pack[node];

    // Eliminate some of the 8 spheres early on
    u32 hit_mask = discriminant_check(ray, pack);
    for (s32 i = 0; i < 8; i++) {
      if (hit_mask & (1 << i)) {
        if (hit_sphere(ray, pack.unpacked[i], t.min, t.max, hi)) {
          t.max   = hi.t;
          any_hit = true;
        }
      }
    }
  }

  return any_hit;
}

} // namespace rt
