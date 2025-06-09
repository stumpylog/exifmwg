cmake_minimum_required(VERSION 3.23)
project(${SKBUILD_PROJECT_NAME} VERSION ${SKBUILD_PROJECT_VERSION})

include(FetchContent)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Global configuration
set(BUILD_SHARED_LIBS
    OFF
    CACHE BOOL "Build a static library for all libraries" FORCE)
set(EXPAT_SHARED_LIBS
    OFF
    CACHE BOOL "Build expat as a static library" FORCE)

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

FetchContent_Declare(
  expat
  GIT_REPOSITORY https://github.com/libexpat/libexpat.git
  GIT_TAG R_2_7_1
  GIT_SHALLOW TRUE
  SOURCE_SUBDIR
  expat)
# We need to trick Exiv2 into seeing our expat
FetchContent_Populate(expat)
add_subdirectory(${expat_SOURCE_DIR}/expat
                 ${CMAKE_CURRENT_BINARY_DIR}/expat-build)
get_target_property(EXPAT_INTERFACE_INCLUDES expat
                    INTERFACE_INCLUDE_DIRECTORIES)
set(EXPAT_INCLUDE_DIR
    "${EXPAT_INTERFACE_INCLUDES}"
    CACHE PATH "Expat include directory from fetched target" FORCE)
if(MSVC)
  set(EXPAT_LIBRARY
      ${CMAKE_CURRENT_BINARY_DIR}/expat-build/Release/expat.lib
      CACHE FILEPATH "Path to expat static library" FORCE)
else()
  set(EXPAT_LIBRARY
      ${CMAKE_CURRENT_BINARY_DIR}/expat-build/libexpat.a
      CACHE FILEPATH "Path to expat static library" FORCE)
endif()

FetchContent_Declare(
  exiv2
  GIT_REPOSITORY https://github.com/Exiv2/exiv2.git
  GIT_TAG v0.28.5
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(exiv2)

# Find Python & nanobind
find_package(
  Python
  COMPONENTS Interpreter Development.Module
  REQUIRED)
find_package(nanobind CONFIG REQUIRED)

# Add the nanobind module
nanobind_add_module(
  ${SKBUILD_PROJECT_NAME}
  NB_STATIC
  NB_SUPPRESS_WARNINGS
  src/${SKBUILD_PROJECT_NAME}/_core.cpp
  src/${SKBUILD_PROJECT_NAME}/models.cpp
  src/${SKBUILD_PROJECT_NAME}/utils.cpp
  src/${SKBUILD_PROJECT_NAME}/parsers.cpp
  src/${SKBUILD_PROJECT_NAME}/writing.cpp
  src/${SKBUILD_PROJECT_NAME}/clearing.cpp
  src/${SKBUILD_PROJECT_NAME}/reading.cpp)

nanobind_add_stub(
  ${SKBUILD_PROJECT_NAME}_stub
  MODULE
  ${SKBUILD_PROJECT_NAME}
  OUTPUT
  ${CMAKE_CURRENT_SOURCE_DIR}/src/${SKBUILD_PROJECT_NAME}/_core.pyi
  DEPENDS
  ${SKBUILD_PROJECT_NAME}
  MARKER_FILE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/${SKBUILD_PROJECT_NAME}/py.typed)

# This is probably a bug in Exiv2 that til header goes here
target_include_directories(${SKBUILD_PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(${SKBUILD_PROJECT_NAME}
                           PRIVATE src/${SKBUILD_PROJECT_NAME}/)

target_link_libraries(${SKBUILD_PROJECT_NAME} PRIVATE Exiv2::exiv2lib)

# Set properties
target_compile_definitions(${SKBUILD_PROJECT_NAME}
                           PRIVATE VERSION_INFO="${SKBUILD_PROJECT_VERSION}")

# Hardening Flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(
    ${SKBUILD_PROJECT_NAME}
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
    ${SKBUILD_PROJECT_NAME}
    PRIVATE /W4
            /WX
            /permissive-
            /GS
            /RTC1
            /SAFESEH
            /NXCOMPAT
            /DYNAMICBASE)
endif()

# Installation
install(TARGETS ${SKBUILD_PROJECT_NAME} DESTINATION .)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/${SKBUILD_PROJECT_NAME}/_core.pyi
              ${CMAKE_CURRENT_SOURCE_DIR}/src/${SKBUILD_PROJECT_NAME}/py.typed
        DESTINATION ${SKBUILD_PROJECT_NAME})
