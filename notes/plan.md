Estimated time: ~4 Months: June - October.


# June - Preparation phase
- [X] Project utilities (logger, file management, error management)
- [X] DirectX 11 renderer pipeline for immediate, debug 2D shapes
- [X] ImGui

Start with the Compute Shaders? What are the downsides of Coding Adventure & 
Ray Tracing in One Weekend? Can I combine them somehow to get something better?

# September 
- [X] Balls rendering
- [X] Materials
- [X] BVH

# October
- [X] Quads
- [X] Lights
- [X] Architecture refactor 
    - [X] uber material
    - [X] replace unordered_map with an array in bvh
    - [X] flatten BVH to an array and use indices instead of pointers
- [ ] Compute Shaders
  - [X] pipeline setup, first shader that outputs a simple texture
  - [X] use indices instead of pointer to the materials in code
  - [X] pack and send RT data to GPU
  - [ ] port CPU code to GPU (should be easy, since we have this rt_loop_balanced already)
    - [?] random number generation
    - [X] Ray, Hit Info
    - [X] hit_sphere
    - [X] materials functions
    - [ ] actual detection loop


@todo: be more specific
- [ ] 3D models form quads

# October
Writing the thesis document.


Miscellaneous 
- [ ] Editor
- [ ] 3D immediate pipeline (to preview objects/scene in editor)
- [ ] Profiling 
- [ ] Optimizing

Simulation aspect:
- flag simulation
- water simulation
- smoke simulation
- some deformation? how about car physics stuff from Kamil?
