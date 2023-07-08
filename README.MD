# Raytracing Engine
Official title: *"3D objects rendering using raytraycing and collision detection algorithms"*  
Supervisor: dr inż. Mateusz Cholewiński
# About
This is my final project for engineering degree from *Electronic and Computer Engineering* at *Wroclaw University of Science and Technology* (2023/2024).    
The goal is to build a renderer which can handle 3D models, lights and materials with
different properties. 
# Tech  
The code uses the following tech stack:
- C++ (I avoid the standard library, though)
- MS Visual Studio Compiler
- DirectX 11
- WinApi
- ...(will add more as the project evolves)

# Building
The project is written in *unity build* philosophy - all files are included into one, big `first.cpp` file, which is then compiled.  
I'm a fan of simple build scripts - you can find them in `scripts/`. You need to update the MSVC compiler path!  
After running one, a `build/` directory should appear with the executable in it. Put it inside the "run_tree".

# License
This code is under [MIT license](LICENSE.MD).