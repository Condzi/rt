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
  - [X] port CPU code to GPU (should be easy, since we have this rt_loop_balanced already)
    - [X] random number generation
    - [X] Ray, Hit Info
    - [X] hit_sphere
    - [X] materials functions
    - [X] actual detection loop
    - [X] debug why tainted shadows do not appear??? reflections weird in general.
      -> issue with random_ functions incorrectly generating vectors
- [X] Compute Shaders continues
  - [X] Quads
  - [X] Working lights example

# November
- [ ] Extra
  - [ ] use iGPU for ImGui and stuff, and dedicated for RT so it wont hang
  - [ ] moving camera using on-screen controls
  - [ ] 3D models from quads?

Miscellaneous 
- [ ] Editor
