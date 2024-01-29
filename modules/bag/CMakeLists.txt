#=================================================================================================
# Copyright (C) 2023-2024 EOLO Contributors
#=================================================================================================

declare_module(NAME bag DEPENDS_ON_MODULES base utils DEPENDS_ON_EXTERNAL_PROJECTS magic_enum tabulate)

find_package(magic_enum REQUIRED)
find_package(tabulate REQUIRED)

add_subdirectory(proto)

# library sources
set(SOURCES
        src/bag.cpp
        README.md
        include/eolo/bag/bag.h)

# library target
define_module_library(
        NAME bag
        PUBLIC_LINK_LIBS
            eolo::base
            eolo::utils
            eolo::bag_proto
            magic_enum::magic_enum
            tabulate::tabulate
        PRIVATE_LINK_LIBS ""
        SOURCES ${SOURCES}
        PUBLIC_INCLUDE_PATHS
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE_INCLUDE_PATHS ""
        SYSTEM_PRIVATE_INCLUDE_PATHS "")

# Subprojects
add_subdirectory(tests)
add_subdirectory(examples)
