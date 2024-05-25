# Tyr: God of valor and justice
[![Build](https://github.com/Immortals-Robotics/Tyr/actions/workflows/build.yml/badge.svg)](https://github.com/Immortals-Robotics/Tyr/actions/workflows/build.yml)
[![C++ format check](https://github.com/Immortals-Robotics/Tyr/actions/workflows/cpp-checks.yml/badge.svg)](https://github.com/Immortals-Robotics/Tyr/actions/workflows/cpp-checks.yml)
[![File name check](https://github.com/Immortals-Robotics/Tyr/actions/workflows/file-name-check.yml/badge.svg)](https://github.com/Immortals-Robotics/Tyr/actions/workflows/file-name-check.yml)
[![Typo check](https://github.com/Immortals-Robotics/Tyr/actions/workflows/typo-check.yml/badge.svg)](https://github.com/Immortals-Robotics/Tyr/actions/workflows/typo-check.yml)

## Building
### Prerequisites
* **CMake** (>3.25)
* **C++ compiler** (supporting C++20). We use *MSVC* on windows, *GCC* on Linux, and *CLang* on macOS
* **vcpkg**
  1. Clone the [repository](https://github.com/microsoft/vcpkg).
  2. Run the bootstrap script (**bootstrap-vcpkg.bat** on windows or **bootstrap-vcpkg.sh** on unix)
  3. Set the **VCPKG_ROOT** environment variable to vcpkg root folder.
* **Python** (>3.7)
* **Ninja** build system
* **git**. It is highly recommended that you use a visual git client, as we have had many problems with people using the cmd line interface.
* Linux-specific **System packages**:
  ```
  zip ninja-build libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev pkg-config libgl1-mesa-dev libx11-dev libxrandr-dev
  ```

We are actively testing on *Windows* and *macOS*, but other systems that meet the requirements should theoretically work fine.

### Build
You should be able to generate build scripts and build the project using one of the CMake presets in CMakePresets.json. But as we primarily use Visual Studio Code as our IDEs. The rest of this readme focuses on them.

#### VS Code
#### Extensions
We have a set of [recommended VS Code extensions](https://code.visualstudio.com/docs/editor/extension-marketplace#_workspace-recommended-extensions) in .vscode/extensions.json. You should get a popup to install them when you open the project. If not, be sure to do it manually.

1. Clone this repo, including its submodules.
2. Open **tyr.code-workspace** in VS Code.
3. Select the desired CMake preset using the **CMake: Select Configure Preset** command.
    - Use *[platform-name]-debug* during development
    - Use *[platform-name]-release* for testing on real robots
4. Build the project using **CMake: Build**.

If everything goes well, you should then see the following line in the *CMake/Build* output:
```
[build] Build finished with exit code 0
```

## Usage
In the *CMake* tab, select the desired launch target and run it using the play button at the bottom of the screen.

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

