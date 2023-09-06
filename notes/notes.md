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
