# 3D Model load with Assimp 
This project contains sample code for loading 3D models using the openGL and Assim lib: it includes how to load meshes, bones, animations, materials from the aiSence data structure in Assimp

## Introduction
Although [LearnOpenGL](https://learnopengl-cn.github.io/03%20Model%20Loading/01%20Assimp/) provides a sample code for loading 3D models using Assimp, it breaks the data structure of the original aiScene when loading. For example, it stores Bones directly in the Model class instead of the Mesh class, which is inconsistent with the design of aiScene. In my opinion , this is not good for people who want to learn Assimp data structure or want to load further complex 3D models.

I refactored the whole loading process based on [LearnOpenGL](https://learnopengl-cn.github.io/03%20Model%20Loading/01%20Assimp/)'s code, and implemented animation playback, texture binding, etc. while keeping the same data structure design idea as Assimp

Due to limited ability and effort, the process I designed may not be the best, but it is still worth trying if you want to explore Assimp's data structure.

## Usage suggestions
* This project does not have any performance optimization or code vulnerability checking and is recommended for learning Assimp data structures.
* The whole project is progressive, as a beginner it is best to start with 1. Basic Load.

## Dependency
I recommend to use vcpkg to install all of them. If you have no idea how to install the dependency, please feel free to ask me via `Issue`.
* [openGL](https://www.opengl.org/)
* [glad](https://github.com/Dav1dde/glad)
* [glfw](https://www.glfw.org/)
* [Assimp](http://assimp.org/)
* [glm](https://glm.g-truc.net/0.9.8/index.html)
* [stb_image](https://github.com/nothings/stb)

`Note that this project does not contain any external library files or header files, you need to install and configure them yourself.`

## Build
### Visual Studio
For developers using Visual Studio, simply open .sln and select a project and build. 

If you have no idea how to run this code by using Visual Studio, please feel free to ask me via `Issue`.

### CMake
Not available at this time, but since the project files are simple, you should be able to build your own.



## Reference
The code is based on [LearnOpenGL](https://learnopengl-cn.github.io/03%20Model%20Loading/01%20Assimp/)