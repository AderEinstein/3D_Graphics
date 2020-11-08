# AEinstein3D Graphics

![AEinstein3D Graphics 11_8_2020 9_51_37 AM](https://user-images.githubusercontent.com/50760776/98472467-242bba80-21a8-11eb-8c46-2a170b336d0a.png)

AEinstein3D Graphics is an Interactive Application and 3D Rendering Engine written in Direct3D11/C++17.

# Prerequisite:
Visual Studio 2019/2017

Windows OS

# Getting Started
Start by cloning the repository with git clone --recursive https://github.com/AderEinstein/3D_Graphics

Open the solution file "3D_Graphics.sln" and run the application from Visual Studio.

# CheckList:
Implemented a specialized interface which creates a window with mouse and keyboard event handling using the WIN32 API.

Implemented a bindable system which manages the binding of components to the rendering pipeline to limit the binding of those components per draw calls and per model.

Implemented z-buffering and a drawable system to render multiple 3D geometric shapes which are allowed to orbit around their center and move freely around the scene.

Implemented image loading and texture mapping.

Interfaced with ImGui Library to spawn gui windows that allow the customization of various components of the scene at runtime.

Implemented a Camera object to navigate the scene.

Implemented dynamic lighting (phong shading) along with a point light drawable object for a lighting simulation.

Procedurally generated geometry meshes and loaded them into the scene to test the efficiency of the current system with a relaxing & interative simulation.

Imported Asset-Import Library to load a wide range of 3D model formats.

# In progress:

Normal Mapping
