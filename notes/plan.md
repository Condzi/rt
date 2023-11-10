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
- [X] Compute Shaders
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

# November
- [X] Compute Shaders continued
  - [X] Quads
  - [X] Working lights example

- [ ] Cleanup / refactor, in preparation for Extras
  - [X] use iterative version of ray_color
  - [X] move out the common code from cpu_rt to rt_base
  - [X] separate cpu_rt from gfx rt creation
  - [ ] refactor the code so it is possible to set parameters in gui before rendering
    - [ ] Camera parameters gui
    - [ ] "Render" GUI button
    
- [ ] Extra
  - [ ] API for the engine so other applications can use this renderer
  - [ ] use iGPU for ImGui and stuff, and dedicated for RT so it wont hang
  - [ ] moving camera using on-screen controls
  - [ ] 3D models
    - [ ] Ray-Triangle intersection (see materials.md)
    - [ ] Naive triangle rendering
    - [ ] .obj parser or something
- [ ] Denoising (see materials.md)?
  - adaptive sampling? https://blog.yiningkarlli.com/2015/03/adaptive-sampling.html
- [ ] Editor?
- [ ] Rendering gifs? (Simple moving camera would be cool)
- [ ] portfolio with custom scenes showcasing all features
- [ ] regression tests (https://blog.yiningkarlli.com/2021/05/porting-takua-to-arm-pt1.html)
  - [ ] compare multisampled images between each other (with some differences because of floating point errors)
  - [ ] compare single-sample images (only for the same platform) 
- [ ] priority based dielectircs
