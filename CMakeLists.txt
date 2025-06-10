cmake_minimum_required(VERSION 3.23)
project(${SKBUILD_PROJECT_NAME} VERSION ${SKBUILD_PROJECT_VERSION})

include(FetchContent)
include(ExternalProject)

# Enable testing
enable_testing()

# We need to trick Exiv2 into seeing our expat
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

# Global configuration
set(BUILD_SHARED_LIBS
    OFF
    CACHE BOOL "Build a static library for all libraries" FORCE)

# --- Exiv2 Configuration Options ---
set(EXIV2_ENABLE_BROTLI
    OFF
    CACHE BOOL "Disable Brotli support in Exiv2" FORCE)
set(EXIV2_BUILD_EXIV2_COMMAND
    OFF
    CACHE BOOL "Disable building exiv2 command line utility" FORCE)
set(EXIV2_ENABLE_VIDEO
    OFF
    CACHE BOOL "Disable building video support" FORCE)
set(EXIV2_ENABLE_LENSDATA
    OFF
    CACHE BOOL "Disable Nikon lens data building" FORCE)
set(EXIV2_ENABLE_WEBREADY
    OFF
    CACHE BOOL "Disable web/HTTP support" FORCE)
set(EXIV2_ENABLE_CURL
    OFF
    CACHE BOOL "Disable curl/HTTP support" FORCE)
set(EXIV2_ENABLE_NLS
    OFF
    CACHE BOOL "Disable native language support" FORCE)
set(EXIV2_ENABLE_INIH
    OFF
    CACHE BOOL "Disable INI file reading" FORCE)
set(EXIV2_ENABLE_BMFF
    OFF
    CACHE BOOL "Disable BMFF support" FORCE)

# --- Expat Configuration Options ---
set(EXPAT_SHARED_LIBS
    OFF
    CACHE BOOL "Build expat as a static library" FORCE)
set(EXPAT_BUILD_TESTS
    OFF
    CACHE BOOL "Disable building Expat tests/benchmarks" FORCE)
set(EXPAT_BUILD_EXAMPLES
    OFF
    CACHE BOOL "Disable building Expat examples" FORCE)
set(EXPAT_BUILD_TOOLS
    OFF
    CACHE BOOL "Disable building Expat tools" FORCE)

# --- zlib Configuration Options ---
set(ZLIB_BUILD_TESTING
    OFF
    CACHE BOOL "Disable building zlib tests" FORCE)
set(ZLIB_BUILD_SHARED
    OFF
    CACHE BOOL "Disable building zlib shared library" FORCE)
set(ZLIB_BUILD_TESTING
    ON
    CACHE BOOL "Enable building a static library" FORCE)

# --- Platform specific configuration Options ---
if(WIN32)
  FetchContent_Declare(
    zlib
    URL https://github.com/madler/zlib/archive/refs/tags/v1.3.1.tar.gz
    URL_HASH
      SHA256=17e88863f3600672ab49182f217281b6fc4d3c762bde361935e436a95214d05c)
  FetchContent_MakeAvailable(zlib)
  set(ZLIB_FOUND TRUE)
  set(ZLIB_LIBRARY zlibstatic)
  set(ZLIB_LIBRARIES zlibstatic)
  set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
  set(ZLIB_INCLUDE_DIRS ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
endif()

FetchContent_Declare(
  expat
  GIT_REPOSITORY https://github.com/libexpat/libexpat.git
  GIT_TAG R_2_7_1
  GIT_SHALLOW TRUE
  SOURCE_SUBDIR expat)

FetchContent_MakeAvailable(expat)

FetchContent_Declare(
  exiv2
  GIT_REPOSITORY https://github.com/Exiv2/exiv2.git
  GIT_TAG v0.28.5
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(exiv2)

# Find Python
find_package(
  Python
  COMPONENTS Interpreter Development.Module
  REQUIRED)
# Find nanobind
find_package(nanobind CONFIG REQUIRED)

# Core implementation sources (no bindings)
set(CORE_SOURCES
    src/your_class.cpp
    src/${SKBUILD_PROJECT_NAME}/KeywordInfoModel.cpp
    src/${SKBUILD_PROJECT_NAME}/XmpAreaStruct.cpp
    src/${SKBUILD_PROJECT_NAME}/DimensionsStruct.cpp
    src/${SKBUILD_PROJECT_NAME}/RegionInfoStruct.cpp
    src/${SKBUILD_PROJECT_NAME}/XmpUtils.cpp
    src/${SKBUILD_PROJECT_NAME}/ImageMetadata.cpp
    src/${SKBUILD_PROJECT_NAME}/utils.cpp
    src/${SKBUILD_PROJECT_NAME}/writing.cpp
    src/${SKBUILD_PROJECT_NAME}/clearing.cpp
    src/${SKBUILD_PROJECT_NAME}/reading.cpp)

# Add the nanobind module
nanobind_add_module(
  bindings
  NB_STATIC
  NB_SUPPRESS_WARNINGS
  STABLE_ABI
  LTO
  src/${SKBUILD_PROJECT_NAME}/bindings.cpp
  ${CORE_SOURCES})

nanobind_add_stub(
  bindings_stub
  MODULE
  bindings
  OUTPUT
  ${CMAKE_CURRENT_SOURCE_DIR}/src/${SKBUILD_PROJECT_NAME}/bindings.pyi
  DEPENDS
  bindings
  MARKER_FILE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/${SKBUILD_PROJECT_NAME}/py.typed)

# This is probably a bug in Exiv2 that this header goes here
target_include_directories(bindings PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(bindings PRIVATE src/${SKBUILD_PROJECT_NAME}/)

target_link_libraries(bindings PRIVATE Exiv2::exiv2lib)

# Set properties
target_compile_definitions(bindings
                           PRIVATE VERSION_INFO="${SKBUILD_PROJECT_VERSION}")

# Hardening Flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(
    bindings
    PRIVATE -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wnull-dereference
            -Wformat=2
            -Wimplicit-fallthrough=5
            -Werror)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(
    bindings
    PRIVATE /W4
            /WX
            /permissive-
            /GS
            /SAFESEH
            /NXCOMPAT
            /DYNAMICBASE)
endif()

# Installation
install(TARGETS bindings DESTINATION "${SKBUILD_PROJECT_NAME}")
install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/${SKBUILD_PROJECT_NAME}/bindings.pyi
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${SKBUILD_PROJECT_NAME}/py.typed
  DESTINATION "${SKBUILD_PROJECT_NAME}")

# Testing
option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
  # Create a static library for testing (core sources only)
  add_library(exifmwg_lib STATIC ${CORE_SOURCES})
  target_include_directories(exifmwg_lib PUBLIC include)
  add_subdirectory(tests)
endif()
