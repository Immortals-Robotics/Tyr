# Tyr: God of valor and justice

[![Build](https://github.com/Immortals-Robotics/Tyr/actions/workflows/build.yml/badge.svg)](https://github.com/Immortals-Robotics/Tyr/actions/workflows/build.yml)
[![C++ format check](https://github.com/Immortals-Robotics/Tyr/actions/workflows/cpp-checks.yml/badge.svg)](https://github.com/Immortals-Robotics/Tyr/actions/workflows/cpp-checks.yml)
[![File name check](https://github.com/Immortals-Robotics/Tyr/actions/workflows/file-name-check.yml/badge.svg)](https://github.com/Immortals-Robotics/Tyr/actions/workflows/file-name-check.yml)
[![Typo check](https://github.com/Immortals-Robotics/Tyr/actions/workflows/typo-check.yml/badge.svg)](https://github.com/Immortals-Robotics/Tyr/actions/workflows/typo-check.yml)

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

## Usage
**TBD**

## Coding standard
We use [cppbestpractices by Jason Turner](https://lefticus.gitbooks.io/cpp-best-practices/content/) as the basis for our standard.

**.clang-tidy** file contains the current naming convention we use. In addition, file and folder names should be ****lower_case****. C++ files should have a **.cpp** extension, C files **.c**, and header files **.h**.

**.clang-format** file contains the current formatting style we use.

Both **.clang-tidy** and **.clang-format** are supported in most IDEs, and can help validate your code according to the standards, and even make necessary changes to be conformant.

## General style
### Indentation
Indent with 4 spaces.

### Spaces
Add spaces after C-style casts. Do not add spaces within angle brackets for template parameters.

### Brace style
Place opening braces on the line after control statements, classes, enums, functions, namespaces, structs, unions, and extern blocks.

### Namespace
Use compact namespaces (`A::B`) when possible.

### Include directives
Group include directives into categories and order them as follows:
  1. Corresponding include (*file.h* for *file.cpp*) (`"..."`)
  2. System includes (`<...>`)
  3. Dependency includes (`<...>`)
  4. Other includes (`"..."`)
  
### Alignment
- Align consecutive assignments and declarations.
- Align operands when breaking expressions.

## Naming style

Ensure consistency and readability by following these naming conventions throughout codebase:

### Files and folders
- Use **lower_case** for file and folder names.

### Namespace
- Use **CamelCase** for namespaces. Root namespace is `Tyr`.

### Types
- Use **CamelCase** for struct, class, enum, union, typedef, and type alias names.

### Functions
- Use **camelBack** naming convention for function names, including global functions.
- Main-like functions can be ignored.

### Fields
- Use **lower_case** for member variables.
- Use `m_` prefix for private and protected member variables.


### Enums
- Use **CamelCase** for both scoped and non-scoped enum constant names.

### Variables
- Use **lower_case** for variables.
- Use `g_` prefix for global variables.
- Use `s_` prefix for static variables.

### Parameters
- Use **lower_case** with `t_` prefix for function parameters.

### Constants
- Use **CamelCase** for all constant (member, static, local, global, constexpr) variables with `k` prefix.

### Macros
- Use **UPPER_CASE** for macro definitions.

## Branching
We use [Github flow](https://docs.github.com/en/get-started/using-github/github-flow) as our branching strategy. Direct commits to the `main` branch are disabled, the goal is to keep it stable and usable.

<img src="https://www.gitkraken.com/wp-content/uploads/2021/03/git-flow.svg" width="300">

### Workflow
1. Create a new branch from main named `dev/your-awesome-dev-task`.
2. Commit changes to your new branch.
3. Open a pull request when you're done. 
4. After your PR is approved and all checks are passed, merge it into the main branch.
5. Delete your dev branch.

## License
This project is licensed under the terms of the GNU GPLv3.

