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

// aka number of objects per leaf
//
s32 constexpr static LEAF_WIDTH = 8;
struct Object_Pack {
  Object_ID unpacked[LEAF_WIDTH];
};

// Shared among all threads -- may cause slowness because of cache
// synchronization?
static std::unordered_map<BVH_Node *, Object_Pack> leaf_to_object_pack;

s32 constexpr static NUM_BINS      = 12;
s32 constexpr static BIN_NOT_FOUND = -1;

struct BVH_Bin {
  s32  size = 0;
  AABB aabb = {};
};

[[nodiscard]] s32
find_best_axis_using_SAH(BVH_Input  *input,
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
      Vec2 const &current_aab_axis = input[i].aabb.v[axis];
      Vec2 const &parent_aabb_axis = parent_aabb.v[axis];

      f32 const t = (current_aab_axis.min - parent_aabb_axis.min) /
                    (parent_aabb_axis.max - parent_aabb_axis.min);

      s32 const bin_idx = std::min(NUM_BINS - 1, (s32)(t * NUM_BINS));

      bins[bin_idx].size++;
      bins[bin_idx].aabb = make_aabb_from_aabbs(bins[bin_idx].aabb, input[i].aabb);
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

static std::unordered_map<uint16_t, Object_Pack> flat_idx_to_obj_pack;

void
flatten_recursive(BVH_Node              *node,
                  std::vector<BVH_Flat> &flattened_tree,
                  uint16_t              &index) {
  if (!node) return;

  uint16_t current_index = index++;
  flattened_tree.push_back(
      {65535, 65535, node->aabb}); // 65535 is used to indicate null pointers

  uint16_t left_index = index;
  if (node->left) {
    flatten_recursive(node->left, flattened_tree, index);
    flattened_tree[current_index].left = left_index;
  }

  uint16_t right_index = index;
  if (node->right) {
    flatten_recursive(node->right, flattened_tree, index);
    flattened_tree[current_index].right = right_index;
  }

  if (!node->left && !node->right) {
    flat_idx_to_obj_pack[current_index] = leaf_to_object_pack.at(node);
  }
}

static s32 best_bin_found     = 0;
static s32 best_bin_not_found = 0;
static s32 empty_slots        = 0;

[[nodiscard]] BVH_Node *
make_BVH_internal(BVH_Input *input, s32 begin, s32 end, AABB const &parent_aabb) {
  // s32 const axis = find_longest_axis(parent_aabb);
  s32 const axis = find_best_axis_using_SAH(input, begin, end, parent_aabb);

  auto comparator = [axis](BVH_Input const &a, BVH_Input const &b) {
    return a.aabb.v[axis].min < b.aabb.v[axis].min;
  };

  BVH_Node *root = perm<BVH_Node>();
  root->aabb     = parent_aabb;

  s32 const object_span = end - begin;
  // leaves CREATION {
  if (object_span <= LEAF_WIDTH) {
    Object_Pack leaf_data;
    // Firstly, populate with invalid boxes
    BVH_Input const &s0 = input[begin];
    for (s32 i = 0; i < LEAF_WIDTH; i++) {
      leaf_data.unpacked[i] = s0.id;
    }

    AABB aabb;
    empty_slots += 8 - object_span;
    for (s32 i = begin; i < end; i++) {
      BVH_Input const &s_i = input[i];

      leaf_data.unpacked[i - begin] = s_i.id;
      aabb = make_aabb_from_aabbs(aabb, s_i.aabb);
    }

    // Leaf case (one object) - both children are NULL and the payload is an object
    // pack.
    //
    root->left           = NULL;
    root->right          = NULL;
    root->aabb                = aabb;
    leaf_to_object_pack[root] = leaf_data;

    return root;
  }
  // } leaves CREATION

  // Divide the objects into bins along the desired axis {
  BVH_Bin bins[NUM_BINS] = {};
  // Populate the bins
  for (s32 i = begin; i < end; i++) {
    Vec2 const &current_aab_axis = input[i].aabb.v[axis];
    Vec2 const &parent_aabb_axis = parent_aabb.v[axis];

    f32 const t = (current_aab_axis.min - parent_aabb_axis.min) /
                  (parent_aabb_axis.max - parent_aabb_axis.min);

    s32 const bin_idx = std::min(NUM_BINS - 1, (s32)(t * NUM_BINS));

    bins[bin_idx].size++;
    bins[bin_idx].aabb = make_aabb_from_aabbs(bins[bin_idx].aabb, input[i].aabb);
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

    auto const binning_comparator = [axis, split_value](BVH_Input const &s) {
      return s.aabb.v[axis].min < split_value;
    };

    mid =
        (s32)(std::partition(input + begin, input + end, binning_comparator) - input);

    // All input fell into one bin - it's not a valid split!
    if (mid == begin || mid == end) {
      best_bin = BIN_NOT_FOUND;
    } else {
      best_bin_found++;
    }
  }

  // Fallback to median split if bin split is not correct
  if (best_bin == BIN_NOT_FOUND) {
    best_bin_not_found++;
    std::sort(input + begin, input + end, comparator);

    mid = begin + object_span / 2;
  }
  // Precalculate AABBs
  AABB left_aabb, right_aabb;
  for (s32 i = begin; i < mid; i++) {
    left_aabb = make_aabb_from_aabbs(left_aabb, input[i].aabb);
  }

  for (s32 i = mid; i < end; i++) {
    right_aabb = make_aabb_from_aabbs(right_aabb, input[i].aabb);
  }

  root->left  = make_BVH_internal(input, begin, mid, left_aabb);
  root->right = make_BVH_internal(input, mid, end, right_aabb);

  /*
    logf("best_bin_found=%d \t best_bin_not_found=%d\n",
         best_bin_found,
         best_bin_not_found);
  */

  // logf("empty_slots=%d\n", empty_slots);

  return root;
}

[[nodiscard]] std::vector<BVH_Flat>
make_BVH(BVH_Input *input, s32 begin, s32 end, AABB const &parent_aabb) {
  BVH_Node *root = make_BVH_internal(input, begin, end, parent_aabb);

  std::vector<BVH_Flat> flat;
  uint16_t              idx = 0;
  flatten_recursive(root, flat, idx);

  return flat;
}

thread_local std::vector<uint16_t> candidates;
void
hit_BVH_internal(std::vector<BVH_Flat> const &bvh,
                 uint16_t                     idx,
                 Vec3 const                  &ray_origin,
                 Vec3 const                  &ray_inv_dir) {
  if (!ray_vs_aabb(
          ray_origin, ray_inv_dir, {.min = 0.001f, .max = FLT_MAX}, bvh[idx].aabb)) {
    return;
  }

  // Potential hit
  if (bvh[idx].left == 65535 && bvh[idx].right == 65535) {
    candidates.push_back(idx);
    return;
  }

  hit_BVH_internal(bvh, bvh[idx].left, ray_origin, ray_inv_dir);
  hit_BVH_internal(bvh, bvh[idx].right, ray_origin, ray_inv_dir);
}

[[nodiscard]] std::vector<Object_ID>
hit_BVH(std::vector<BVH_Flat> const &bvh, Ray const &ray) {
  candidates.reserve(128);
  candidates.clear();

  hit_BVH_internal(bvh, 0, ray.origin, ray.direction_inv);

  std::vector<Object_ID> result;
  result.reserve(candidates.size() * LEAF_WIDTH);
  for (uint16_t node : candidates) {
    auto const &pack = flat_idx_to_obj_pack[node];
    std::copy(pack.unpacked, pack.unpacked + LEAF_WIDTH, std::back_inserter(result));
  }

  return result;
}

} // namespace rt
