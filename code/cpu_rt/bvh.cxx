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
s32 constexpr static LEAF_WIDTH = 4;
using Leaf_Data                 = std::array<Sphere, (size_t)LEAF_WIDTH>;
static std::unordered_map<BVH_Node *, Leaf_Data> leaf_to_sphere;

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
    Leaf_Data leaf_data;
    leaf_data.fill(spheres[begin]); // Always fill the entire array.

    AABB aabb;
    for (s32 i = begin; i < end; i++) {
      leaf_data[i - begin] = spheres[i];
      aabb                 = make_aabb_from_aabbs(aabb, spheres[i].aabb);
    }

    // Leaf case (one object) - both children are NULL and the payload is a sphere.
    root->left           = NULL;
    root->right          = NULL;
    root->aabb           = aabb; // is this aabb ok?
    leaf_to_sphere[root] = leaf_data;

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

  logf("best_bin_found=%d \t best_bin_not_found=%d\n",
       best_bin_found,
       best_bin_not_found);

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

[[nodiscard]] bool
hit_BVH(BVH_Node *root, Ray const &ray, Vec2 t, Hit_Info &hi) {
  candidates.reserve(128);
  candidates.clear();

  hit_BVH_internal(root, ray.origin, ray.direction_inv);

  bool any_hit = false;
  for (BVH_Node *node : candidates) {
    Leaf_Data const &leaf_data = leaf_to_sphere[node];
    for (s32 i = 0; i < LEAF_WIDTH; i += 2) {
      if (hit_sphere(ray, leaf_data[i], t.min, t.max, hi)) {
        t.max   = hi.t;
        any_hit = true;
      }

      if (hit_sphere(ray, leaf_data[i + 1], t.min, t.max, hi)) {
        t.max   = hi.t;
        any_hit = true;
      }
    }
  }

  return any_hit;
}

} // namespace rt
