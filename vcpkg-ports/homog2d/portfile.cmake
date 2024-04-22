include(vcpkg_common_functions)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO skramm/homog2d
    REF a41e490b68d3ce1a344aef0e8898936adee1b2cc
    SHA512 0ec76436ea085575912ae6d555153eb5abed3fb54516a34060be49e4a5c10f14fff950a5af196c10f4a770e8ce61b010f5a909b52a47c6cc5dcf68950ef21ad6
    HEAD_REF master
)

file(COPY "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt" DESTINATION "${SOURCE_PATH}")

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
)

vcpkg_install_cmake()

file(INSTALL ${SOURCE_PATH}/LICENCE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)
