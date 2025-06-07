cmake_minimum_required(VERSION 3.18)
project(${SKBUILD_PROJECT_NAME} VERSION ${SKBUILD_PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXIV2_ENABLE_BROTLI OFF CACHE BOOL "Disable Brotli support in Exiv2" FORCE)
set(EXIV2_BUILD_EXIV2_COMMAND OFF CACHE BOOL "Disable building exiv2 command line utility" FORCE)
set(EXIV2_ENABLE_VIDEO OFF CACHE BOOL "Disable building video support" FORCE)
set(EXIV2_ENABLE_LENSDATA OFF CACHE BOOL "Disable Nikon lens data building" FORCE)

# Find Python & nanobind
find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
find_package(nanobind CONFIG REQUIRED)

add_subdirectory(extern/exiv2)

# --- KEY ADDITION ---
# Create a custom "gatekeeper" target. This target builds nothing itself.
# Its sole purpose is to depend on exiv2lib and serve as an explicit
# synchronization point in the build graph for Ninja.
add_custom_target(exiv2_build_gate DEPENDS exiv2lib)

# Add the nanobind module
nanobind_add_module(
    ${SKBUILD_PROJECT_NAME}
    LTO
    src/${SKBUILD_PROJECT_NAME}/_core.cpp
)

# Add include paths and link libraries as before
target_include_directories(${SKBUILD_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/exiv2/include
    ${CMAKE_CURRENT_BINARY_DIR}/extern/exiv2/include
		${CMAKE_BINARY_DIR}
)
target_link_libraries(${SKBUILD_PROJECT_NAME} PRIVATE exiv2lib)

# Now, make your module depend on the gatekeeper target instead of directly
# on exiv2lib.
add_dependencies(${SKBUILD_PROJECT_NAME} exiv2_build_gate)

# Set properties
target_compile_definitions(${SKBUILD_PROJECT_NAME} PRIVATE VERSION_INFO="${SKBUILD_PROJECT_VERSION}")

# Hardening Flags
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${SKBUILD_PROJECT_NAME} PRIVATE
        -Wall
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
        -Werror
        -fstack-protector-strong
        -fPIE
        -D_FORTIFY_SOURCE=2
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${SKBUILD_PROJECT_NAME} PRIVATE
        /W4
        /WX
        /permissive-
        /GS
        /RTC1
        /SAFESEH
        /NXCOMPAT
        /DYNAMICBASE
    )
endif()

# Installation
install(TARGETS ${SKBUILD_PROJECT_NAME} LIBRARY DESTINATION .)
