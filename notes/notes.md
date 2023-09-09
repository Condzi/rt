# Cppcheck?
CppCheck gives false positives about snprintf. Maybe it would be better to have
clang-tidy or something simillar, instead. SonarQube would be dope.

# Panic
I see that often we have "if (!x) errf(...)" pattern ("panic" situations).
Maybe add a macro with usage like "panic_if(x, "Value was %d, expected 123", 321)"?
It would work like check_, but would halt the program and give an error box with info.
Could also be a [[noreturn]] function call at this point.

# Printf / Sprintf
Because libc is bad at handling Unicode, I thought about rewriting these functions.
Main ideas:
- use templates to get rid of formatting arguments, so the call can may look like:
  `tprint("Value of '%' is %.", "my_number", 123);
- no need to write own formatting for everything, just handle our String separately
  and forward other arguments to libc sprintf.

# Unit tests
Use ChatGPT for generating UTs for math functions & other code. Maybe roll out our
own simple UT library and add the testing as a step in GitHub pipeline?

# Window procedure
Window procedure handles (mainly) 3 types of messages: window-related, input-related 
and rendering-related (special case of win-related, for example window resizing).
The idea now is that we have a one, main handler, which calls subhandlers from
different modules (window module, input module and rendering module). If the sub-handlers
don't handle the message, we fallback to the default window handler and that's it.
This will prevent the window module from being too specific / tied to other modules.

# Rewriting 
Now, since we know how the raytracing actually looks like, we can plan a better
data and code layout.
1. BVH is accessing AABB. Let's keep them together.
2. This implementation traces entire ray path every time. Can we trace up until the first obstacle instead, and then trace again? This seem to be better for compute shaders and parallelism since it would avoid recursive calls and the image would be build 1 frame at a time, not pixel by pixel...
I see this as rendering in stages:
  0. Cast rays from camera
  1. Gather all ray intersections into a container
  2. Assign the colors to the rays by fetching the materials
  3. If ray reflection depth < MAX_DEPTH, cast a new ray. Go to 1.
  4. Otherwise, end.

Potential problems: many rays at once? So we would only small patches at a time?
Math: image 1280x720px = 921'600px 500 samples/px => 921600*500=460'800'000!!
Not worth caching? Or maybe do patches which would fit in L3 cache line (4MB)?
Ray data:
  - Vec3 start
  - Vec3 direction
  - Vec3 direction_inv
  - s32 depth
  ----------------
  12 + 12 + 12 + 4 = 40 bytes per ray = 4 000 000/40 = 100 000 rays would fin in L3? Less, because we also need to load AABBs... and Sphere... so maybe like  50k? x8 threads

## Better idea - only aabb
Focus only on the AABB stage? 
1. Take the BVH tree.
2. Take ray origin, inv_dir and 't'
3. Generate list of leaves that had been hit (AABB intersected)
4. After doing that enough (how many?) times, do the actual ray-shape tests (in a separate loop).

This approach separetes two code paths but generates a lot of data... Generating the AABB-ray intersection results can eat all potential profits :/ And bloats
code too?

potential problems:
- editor

# To do / priority

- CPU Raytraycer (based on "rt in one weekend")
  - lights
  - 3D models?

- Wrapping up
  - performance benchmark (low hanging fruits at least)
  - code refactoring

- Moving to compute shaders

- UTF-8, use String everywhere & rewrite the tprint to accept it.

- hw info (simd availability, threads) from Agner Fog

- unit test for math module
- translate build scripts to shell?
- 3D debug shapes?! -- need 3D math
