
# Noise Based Foliage Rendering
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) ![OpenGL](https://img.shields.io/badge/OpenGL-FFFFFF?style=for-the-badge&logo=opengl)
![teaser](https://github.com/user-attachments/assets/64bbcb7c-a9df-4652-8d6c-92ecd28ca542)

This project is a Noise Based Foliage Rendering grass system which provides big fields of grass based on compute shaders and GPU instancing. This document provides instructions for setting up and building this application. 

For a visual look, check out: https://www.youtube.com/watch?v=Eufx7EyKU48


## Building this project

To build this project should be fairly straight forward if VS has been properly set up for C++. 

1. Extract the zip file and then open the sln in visual studio. 
2. Open the MainProject folder in the solution explorer and right click GrassRendering and set it as Sartup Project.
3. In the same menu, select project dependencies and select nclgl for GrassRendering
4. Optional: Right click, on grass rendering > properties > C/C++ > Additional Include Directories and go to THIRD PARTY/IMGUI 
5. Run the project



## Feature List
* C++, OpenGL, GLSL, Shader work, and ImGUI.
* Customizeable grass amount, Large and small, upto 2 million individual grass blades.
* Inbuilt weather wind system with smooth blending between animations, controls based on wind speed, direction, and thickness. 
* Clumping support, and camera distance blending.
* Height based visual differences, and color correction.
* In built brightness, saturation, gamma correction, and contrast controls.
* Compute shader with instance rendering and toolset to evaluate perfromance. 

## Contact

If you have any reason to reach out, please do so at starmandev@gmail.com

