namespace rt {
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
    root->sphere = &spheres[begin];
    return root;
  }

  root->left  = (BVH_Node *)alloc_perm(sizeof(BVH_Node));
  root->right = (BVH_Node *)alloc_perm(sizeof(BVH_Node));

  if (object_span == 2) {
    // Leaf case (two objects) -- we create children as leafs with NULL children
    root->left->left = root->left->right = NULL;
    root->right->left = root->right->right = NULL;
    if (comparator(spheres[begin], spheres[begin + 1])) {
      root->left->sphere  = &spheres[begin];
      root->right->sphere = &spheres[begin + 1];
    } else {
      root->left->sphere  = &spheres[begin + 1];
      root->right->sphere = &spheres[begin];
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

[[nodiscard]] bool
hit_BVH(BVH_Node *root, Ray const &ray, Vec2 t, Hit_Info &hi) {
  // Edge case -- it's a leaf
  if (root->left == NULL && root->right == NULL) {
    Sphere *sphere = root->sphere;

    bool const hit_aabb = ray_vs_aabb(ray.origin, ray.direction, t, sphere->aabb);
    if (!hit_aabb) return false;

    bool const hit_model = hit_sphere(ray, *sphere, t.min, t.max, hi);
    return hit_model;
  }

  if (!ray_vs_aabb(ray.origin, ray.direction, t, root->aabb)) {
    return false;
  }

  bool const hit_left  = hit_BVH(root->left, ray, t, hi);
  t.max                = (hit_left) ? (hi.t) : (t.max);
  bool const hit_right = hit_BVH(root->right, ray, t, hi);

  return hit_left || hit_right;
}

} // namespace rt
