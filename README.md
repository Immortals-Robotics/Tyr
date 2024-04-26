# Loki, God of mischief

[![Build](https://github.com/Immortals-Robotics/Loki/actions/workflows/build.yml/badge.svg)](https://github.com/Immortals-Robotics/Loki/actions/workflows/build.yml)

## Building
We actively test on Windows and macOS, but other systems that meet the requirements should in theory work fine.

To build the project you need:
* CMake (>3.25)
* A recent C++ compiler supporting C++20
* Python (>3.7)
* (*optional*) Ninja build system
* git

On Ubuntu, you need to install the following system packages:
```shell
sudo apt install zip ninja-build libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev pkg-config libgl1-mesa-dev libx11-dev libxrandr-dev
```

Also make sure to get submodules in the repo.

Then you should be able to generate build scripts and build the project using one of the CMake presets in CMakePresets.json.

## **Usage**
**TBD**

## **Coding standard**
We use [cppbestpractices by Jason Turner](https://lefticus.gitbooks.io/cpp-best-practices/content/) as the basis for our standard.

**.clang-tidy** file contains the current naming convention we use. In addition, file and folder names should be **lower_case**. C++ files should have a **.cpp** extension, C files **.c**, and header files **.h**.

**.clang-format** file contains the current formatting style we use.

Both **.clang-tidy** and **.clang-format** are supported in most IDEs, and can help validate your code according to the standards, and even make necessary changes to be conformant.

## **License**
This project is licensed under the terms of the GNU GPLv3.

