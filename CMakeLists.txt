# =================================================================================================
# Copyright (C) 2023-2024 HEPHAESTUS Contributors
# =================================================================================================

cmake_minimum_required(VERSION 3.22.1)
project(hephaestus LANGUAGES CXX C)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(BUILD_AS_SUBPROJECT FALSE)
else()
  set(BUILD_AS_SUBPROJECT TRUE)
endif()

if(NOT BUILD_AS_SUBPROJECT)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# Integrate the Hephaestus Build System to setup the project
include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)
