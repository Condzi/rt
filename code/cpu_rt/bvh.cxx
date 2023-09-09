namespace rt {
static std::unordered_map<BVH_Node *, Sphere> node_to_sphere;

[[nodiscard]] BVH_Node *
make_BVH(Sphere *spheres, s32 begin, s32 end, AABB const &parent_aabb) {
  int axis = [parent_aabb] {
    f32 const x = len_sq(parent_aabb.x);
    f32 const y = len_sq(parent_aabb.y);
    f32 const z = len_sq(parent_aabb.z);

    if (x > y && x > z) return 0;
    if (y > x && y > z) return 1;
    return 2;
  }();

  auto comparator = [axis](Sphere const &a, Sphere const &b) {
    switch (axis) {
      case 0:
        return a.aabb.x.min < b.aabb.x.min;
      case 1:
        return a.aabb.y.min < b.aabb.y.min;
      case 2:
        return a.aabb.z.min < b.aabb.z.min;
      default:
        assert(false);
    }
    return false;
  };

  BVH_Node *root = (BVH_Node *)alloc_perm(sizeof(BVH_Node));
  root->aabb     = parent_aabb;

  s32 object_span = end - begin;
  if (object_span == 1) {
    // Leaf case (one object) - both children are NULL and the payload is a sphere.
    root->left   = NULL;
    root->right  = NULL;
    root->aabb           = spheres[begin].aabb;
    node_to_sphere[root] = spheres[begin];

    return root;
  }

  root->left  = (BVH_Node *)alloc_perm(sizeof(BVH_Node));
  root->right = (BVH_Node *)alloc_perm(sizeof(BVH_Node));

  if (object_span == 2) {
    // Leaf case (two objects) -- we create children as leafs with NULL children
    root->left->left = root->left->right = NULL;
    root->right->left = root->right->right = NULL;
    if (comparator(spheres[begin], spheres[begin + 1])) {
      root->left->aabb            = spheres[begin].aabb;
      root->right->aabb           = spheres[begin + 1].aabb;
      node_to_sphere[root->left]  = spheres[begin];
      node_to_sphere[root->right] = spheres[begin + 1];

    } else {
      root->left->aabb  = spheres[begin + 1].aabb;
      root->right->aabb = spheres[begin].aabb;

      node_to_sphere[root->left]  = spheres[begin + 1];
      node_to_sphere[root->right] = spheres[begin];
    }

  } else {
    std::sort(spheres + begin, spheres + end, comparator);

    auto mid    = begin + object_span / 2;
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
  }

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
    Sphere const &sphere = node_to_sphere[node];
    if (hit_sphere(ray, sphere, t.min, t.max, hi)) {
      any_hit = true;
      t.max   = hi.t;
    }
  }

  return any_hit;
}

} // namespace rt
