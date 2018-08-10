# Vulkan Samples
  - This fork of the LunarG Vulkan Samples repo contains the source for performance tests that looked at the difference in performance between primary and secondary command buffer overhead across different platforms
  - This source has since been moved into the ANGLE project repository as part of their performance test suite at [this link](https://github.com/google/angle/tree/master/src/tests/perf_tests) in the file VulkanCommandBufferPerf.cpp
    - The utility functions can be found at [this link](https://github.com/google/angle/tree/master/src/tests/test_utils) under third_party/*
  - The build instructions below are still relevent if you would like to build and run the tests yourself
  - The samples have been cut down to just 15-draw_cube and draw-textured-cube (and some unfinished work in 13-vertex_buffer)
  - Benchmark implementations can be found in the API-Samples/utils/draw_benchmarks.cpp file
  
  
## Linux Dependencies

Several packages are needed to be able to build the samples under Linux.  Execute:
```sudo apt-get install git cmake build-essential bison libx11-xcb-dev libxkbcommon-dev libmirclient-dev libwayland-dev libxrandr-dev```

## Clone the Repository

The VulkanSamples repository also includes the Vulkan loader, validation layers, header files, and associated tests.
These files are mirrored from this GitHub repository:
https://github.com/KhronosGroup/Vulkan-LoaderAndValidationLayers
These components are required to enable this repository to be built standalone; that is without having to clone the Vulkan-LoaderAndValidationLayers repository.

To create your local git repository of VulkanSamples:
```
cd YOUR_DEV_DIRECTORY
git clone git@github.com:LunarG/VulkanSamples.git
cd VulkanSamples
# This will fetch and build glslang and spriv-tools
./update_external_sources.sh         # linux
update_external_sources.bat --all  # windows
```
## Linux Build

Example debug build:
```
cd YOUR_DEV_DIRECTORY/VulkanSamples  # cd to the root of the VulkanSamples git repository
cmake -H. -Bdbuild -DCMAKE_BUILD_TYPE=Debug
cd dbuild
make
```
## Android:
- Build shaderc source code inside NDK
```java
$ cd ${ndk_root}/sources/third_party/shaderc
$ ../../../ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=Android.mk APP_STL:=c++_static APP_ABI=all NDK_TOOLCHAIN_VERSION:=clang libshaderc_combined -j16
```
- Generate Android Studio Projects
```java
$ cd YOUR_DEV_DIRECTORY/VulkanSamples/API-Samples
$ cmake -DANDROID=ON
```
- Import VulkanSamples/API-Samples/android/build.gradle into Android Studio 2.3.0+.

## Windows System Requirements

Windows 7+ with additional required software packages:

- Microsoft Visual Studio 2013 Professional.  Note: it is possible that lesser/older versions may work, but that has not been tested.
- CMake (from http://www.cmake.org/download/).  Notes:
  - Tell the installer to "Add CMake to the system PATH" environment variable.
- Python 3 (from https://www.python.org/downloads).  Notes:
  - Select to install the optional sub-package to add Python to the system PATH environment variable.
  - Need python3.3 or later to get the Windows py.exe launcher that is used to get python3 rather than python2 if both are installed on Windows
- Git (from http://git-scm.com/download/win).
  - Tell the installer to allow it to be used for "Developer Prompt" as well as "Git Bash".
  - Tell the installer to treat line endings "as is" (i.e. both DOS and Unix-style line endings).
- glslang is required to compile glsl to spirv and should be obtained by running update_external_sources

## Windows Build

To build all Windows targets (e.g. in a "Developer Command Prompt for VS2013" window):
```
cd VulkanSamples  # cd to the root of the VulkanSamples git repository
mkdir build
cd build
cmake -G "Visual Studio 12 Win64" ..
```
Use the major version number reported by Visual Studio in Help->About.

At this point, you can use Windows Explorer to launch Visual Studio by double-clicking on the "VULKAN.sln" file in the \build folder.  
Once Visual Studio comes up, you can select "Debug" or "Release" from a drop-down list.  
You can start a build with either the menu (Build->Build Solution), or a keyboard shortcut (Ctrl+Shift+B).
As part of the build process, Python scripts will create additional Visual Studio files and projects,
along with additional source files.  
All of these auto-generated files are under the "build" folder.

Vulkan programs must be able to find and use the Vulkan-1.dll library.
Make sure it is either installed in the C:\Windows\System32 folder,
or the PATH environment variable includes the folder that it is located in.

### Windows 64-bit Installation Notes
If you plan on creating a Windows Install file (done in the windowsRuntimeInstaller sub-directory) you will need to build for both 32-bit and 64-bit Windows since both versions of EXEs and DLLs exist simultaneously on Windows 64.

To do this, simply create and build the release versions of each target:
```
cd VulkanSamples  # cd to the root of the Vulkan git repository
mkdir build
cd build
cmake -G "Visual Studio 12 Win64" ..
msbuild ALL_BUILD.vcxproj /p:Platform=x64 /p:Configuration=Release
mkdir build32
cd build32
cmake -G "Visual Studio 12" ..
msbuild ALL_BUILD.vcxproj /p:Platform=x86 /p:Configuration=Release
```
## How to Build and Run

[BUILD.md](BUILD.md)
Includes directions for building all components as well as running validation tests and demo applications.

Information on how to enable the various Validation layers is in
[layers/README.md](layers/README.md).

Architecture and interface information for the loader is in
[loader/LoaderAndLayerInterface.md](loader/LoaderAndLayerInterface.md).

## License
This work is released as open source under a Apache-style license from Khronos including a Khronos copyright.

See COPYRIGHT.txt for a full list of licenses used in this repository.

## Acknowledgements
While this project has been developed primarily by LunarG, Inc., there are many other
companies and individuals making this possible: Valve Corporation, funding
project development; Google providing significant contributions to the validation layers;
Khronos providing oversight and hosting of the project.
>>>>>>> trunk
