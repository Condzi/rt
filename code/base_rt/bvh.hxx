/**
 * BVH - bounding volume hierarchy. Data structure used for optimizing ray-object
 * intersections.
 * It works by grouping bounding volumes, from smallest to the largest (scene).
 */

namespace rt {
struct BVH_Node {
  BVH_Node *left, *right;
  // If node - aabb, if leaf (left=right=NULL) - sphere
  AABB aabb;
};

struct BVH_Flat {
  uint16_t left, right;
  AABB     aabb;
};

struct BVH_Input {
  Object_ID id;
  AABB      aabb;
};

[[nodiscard]] std::vector<BVH_Flat>
make_BVH(BVH_Input *input, s32 begin, s32 end, AABB const &parent_aabb);

// Returns a list of potential contacts.
//
[[nodiscard]] std::vector<Object_ID>
hit_BVH(std::vector<BVH_Flat> const &bvh, Ray const &ray);
} // namespace rt
