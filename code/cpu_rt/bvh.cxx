namespace rt {
[[nodiscard]] BVH_Node *
make_BVH(Sphere *spheres, s32 begin, s32 end) {
  int  axis       = random_s32_in_range(0, 2);
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

  s32       object_span = end - begin;
  BVH_Node *root        = (BVH_Node *)alloc_perm(sizeof(BVH_Node));

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
    root->aabb =
        make_aabb_from_aabbs(root->left->sphere->aabb, root->right->sphere->aabb);
  } else {
    std::sort(spheres + begin, spheres + end, comparator);

    auto mid    = begin + object_span / 2;
    root->left  = make_BVH(spheres, begin, mid);
    root->right = make_BVH(spheres, mid, end);

    root->aabb = make_aabb_from_aabbs(root->left->aabb, root->right->aabb);
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
