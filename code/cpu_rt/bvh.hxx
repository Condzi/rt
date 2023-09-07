/**
 * BVH - bouding volume hiearchy. Data structure used for optimizing ray-object
 * intersections.
 * It works by grouping bounding volumes, from smallest to the largest (scene).
 */

namespace rt {
struct Sphere;
struct BVH_Node {
  BVH_Node *left, *right;
  // If node - aabb, if leaf (left=right=NULL) - sphere
  union {
    AABB    aabb;
    Sphere *sphere = NULL;
  };
};

[[nodiscard]] BVH_Node *
make_BVH(Sphere *spheres, s32 begin, s32 end);

[[nodiscard]] bool
hit_BVH(BVH_Node *root, Ray const &ray, Vec2 t, Hit_Info &hi);
} // namespace rt