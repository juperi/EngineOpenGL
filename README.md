# C++ OpenGL Graphical Engine

This repository contains an OpenGL Graphical Engine in C++, for the Advanced Programming I course from the Master in Game Design and Development at Valencia International University (VIU).

## Getting Started

These instructions will get you a copy of the project up and running on your local machine using Visual Studio 2017.

### Prerequisites

* [Visual Studio 2017](https://visualstudio.microsoft.com/es/downloads/?rr=https%3A%2F%2Fwww.google.com%2F) - IDE.
* [GENie](https://github.com/bkaradzic/GENie) - Project generator.


### Built With

* [GLFW](http://www.glfw.org/) - Window, Render Context, Input.
* [GLAD](http://glad.dav1d.de/) - OpenGL Functions.
* [GLM](https://glm.g-truc.net/0.9.9/index.html) - Mathematic Library.
* [STB](https://github.com/nothings/stb) - Load images Library.
* [ASSIMP](http://www.assimp.org/index.php/downloads) - Open Asset Importer Library.

### Set Up

A step by step series of examples that tell you how to get the environment running.

1. Place the GENie .exe file in the project folder.
2. From the project folder, open a console and run the following command: 
  ```
  ./genie.exe vs2017
  ```
3. A build/ folder has been generated. Find inside the .sln file to run the project.

### Additional information

From AG09, assimp libs has been added. Follow the next steps to configure MODELS LOAD and avoid crashes:

1. Download [CMake](https://cmake.org/download/)

2. Export assimp and check that inside the folder exists. NOTE: Do the export outside "MyProject" folder.
	- CMakeLists.txt
	- include/assimp

3. Open CMake: 
	- Set source code: myPath/assimp-4.1.0
	- Set where to build binaries: myPath/assimp-4.1.0/build
	- Configure:
		* Generator: Visual Studio 15 2017 Win64
		* Use default native compilers
	- Check BUILD_SHARED_LIBS
	- Uncheck ASSIMP_BUILD_SAMPLES, ASSIMP_BUILD_TESTS, BUILD_TESTING
	- Generate (twice just in case)
	- Open Project

4. In the Visual Studio 2017:
	- Build assimp in "Debug" "x64"
	- Build assimp in "Release" "x64"

5. Check both "Debug" and "Release" folders have been generated inside "myPath/assimp-4.1.0/build/code"

6. Copy the .dll file from "Debug" (around 20MB) inside MyProject/bin. 
   NOTE: Copy the "Release" .dll when you need to work in Release mode.

7. Create a "libs" folder inside MyProject and copy there "Debug" and "Release" folders from "assimp-4.1.0/build/code"
