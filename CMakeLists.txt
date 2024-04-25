cmake_minimum_required(VERSION 3.22)

set(USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

option(USE_UNITY_BUILDS "Enable unity build to improve build times" OFF)

project(loki)

add_subdirectory(source/protos)
add_subdirectory(source/common)
add_subdirectory(source/gui)
