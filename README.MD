# Ray Tracing Engine
Thesis: *"3D objects rendering using ray tracing and collision detection algorithms"*  
Supervisor: dr inż. Mateusz Cholewiński
# About
This is my final project for engineering degree from *Electronic and Computer Engineering* at *Wroclaw University of Science and Technology* (2023/2024).    
The goal is to build a renderer which can handle 3D shapes, lights and materials with
different properties. 
# Tech  
The code uses the following tech stack:
- C++ (I avoid the standard library, though)
- MS Visual Studio Compiler
- DirectX 11
- WinApi
- ...(will add more as the project evolves)

# Building
Use CMake to build. Currently the app assumes that it works in `W:\\pwr\engineering-thesis\\rt\\run_tree`.
The project is written in *unity build* philosophy - all files are included into one, big `first.cpp` file, which is then compiled.  
 Project is extremely simple and self-contained, so there should be no issues. If you encounter any problems, don't hasitate to ask.

# License
This code is under [MIT license](LICENSE.MD).
