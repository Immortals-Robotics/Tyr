vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO raylib-extras/rlImGui
    REF d765c1ef3d37cf939f88aaa272a59a2713d654c9
    SHA512 914b7c20f706bc759961bcf851a60798142e911c0d9d2188ebdad7392518f9a070f69db43d0d81684dbde12c342eca4f002cb4246ae736e1d6ad97d705f0884d
    HEAD_REF main
)

file(COPY "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt" DESTINATION "${SOURCE_PATH}")

vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}
    OPTIONS
        -DBUILD_EXAMPLES=OFF)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME "rlimgui")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/extras/FontAwsome_LICENSE.txt")
