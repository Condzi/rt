CppCheck gives false positives about snprintf. Maybe it would be better to have
clang-tidy or something similar, instead. SonarQube would be dope.

I see that often we have "if (!x) errf(...)" pattern ("panic" situations).
Maybe add a macro with usage like "panic_if(x, "Value was %d, expected 123", 321)"?
It would work like check_, but would halt the program and give an error box with info.
Could also be a [[noreturn]] function call at this point.

- add pathf!!!

- seh
- minidump

- math: vectors, matrices + unit tests for them!

- window creation
- DirectX 11 immediate pipeline

- hw info (simd availability, threads) from Agner Fog



- translate build scripts to shell?
