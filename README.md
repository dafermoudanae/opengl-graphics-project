# opengl-graphics-project

Interactive 2D and 3D graphics application in C++ with OpenGL and GLSL shaders, developed in three parts.

Assignment, Computer Graphics and Interaction Systems, University of Ioannina, 2025-26.

## Contents
- `project part1/`: 2D/3D graphics part 1 (`Source-1A.cpp`, vertex and fragment shaders, report).
- `project part2/`: part 2 (`Source-1B.cpp`, vertex and fragment shaders, report, demo video `demo52065319.mp4`).
- `project part3/`: part 3 (`Source-1C.cpp`, vertex and fragment shaders, report).

Each part contains its own report as a PDF.

## Requirements
- C++ compiler (the code uses `windows.h`, so it targets Windows)
- GLEW
- GLFW
- GLM

## How to Run
1. Clone the repository: `git clone https://github.com/dafermoudanae/opengl-graphics-project.git`
2. Open the folder of the part you want to run (for example `project part1`) in your IDE.
3. Link the project with GLEW, GLFW and GLM.
4. Keep the shader files (`.vertexshader` and `.fragmentshader`) in the same folder as the executable, because the program loads them from there.
5. Build and run the program.

## Demo
The video `project part2/demo52065319.mp4` shows the application of part 2 running.
