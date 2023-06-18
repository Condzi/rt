# Cppcheck?
CppCheck gives false positives about snprintf. Maybe it would be better to have
clang-tidy or something similar, instead. SonarQube would be dope.

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

# To do / priority
- math: vectors, matrices + unit tests for them!
  - this is just for the 2D part (debug shapes / textures), for 3D we'll need more
    stuff

- window creation
- DirectX 11 immediate pipeline
  - simple texture rendering (for CPU raytraycer)
  - 2D debug shapes
  - 3D debug shapes?! -- need 3D math
- UTF-8, use String everywhere & rewrite the tprint to accept it.

- hw info (simd availability, threads) from Agner Fog

- translate build scripts to shell?
