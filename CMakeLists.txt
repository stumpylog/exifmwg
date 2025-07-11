cmake_minimum_required(VERSION 3.24)
project(${SKBUILD_PROJECT_NAME} VERSION ${SKBUILD_PROJECT_VERSION})

include(FetchContent)
include(ExternalProject)

# Enable testing
enable_testing()

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

# Global options
option(BUILD_TESTING "Build tests" OFF)
option(ENABLE_COVERAGE "Enable coverage target" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy checks" OFF)
option(ENABLE_STATIC_ANALYZER "Enable clang static analyzer" OFF)

# Global configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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

if(WIN32)
  set(EXIV2_ENABLE_PNG
      OFF
      CACHE BOOL "Disable PNG support on Windows (zlib issues)" FORCE)
endif()

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
set(ZLIB_BUILD_MINIZIP
    OFF
    CACHE BOOL "Disable building libminizip" FORCE)
set(ZLIB_INSTALL
    OFF
    CACHE BOOL "Disable zlib install" FORCE)
set(ZLIB_BUILD_STATIC
    ON
    CACHE BOOL "Enable building a static library" FORCE)

# Fetch dependencies
FetchContent_Declare(
  EXPAT
  GIT_REPOSITORY https://github.com/libexpat/libexpat.git
  GIT_TAG R_2_7_1
  GIT_SHALLOW TRUE
  SOURCE_SUBDIR expat OVERRIDE_FIND_PACKAGE)

FetchContent_MakeAvailable(expat)

get_target_property(_expat_includes expat INTERFACE_INCLUDE_DIRECTORIES)
set(EXPAT_INCLUDE_DIRS ${_expat_includes})
add_library(EXPAT::EXPAT ALIAS expat)

if(WIN32)
  FetchContent_Declare(
    Zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG v1.3.1
    GIT_SHALLOW TRUE
    OVERRIDE_FIND_PACKAGE)
  FetchContent_MakeAvailable(Zlib)
  add_library(ZLIB::ZLIB ALIAS zlibstatic)
endif()
FetchContent_Declare(
  exiv2
  GIT_REPOSITORY https://github.com/Exiv2/exiv2.git
  GIT_TAG v0.28.5
  GIT_SHALLOW TRUE
  PATCH_COMMAND git apply --ignore-space-change --ignore-whitespace
                ${CMAKE_CURRENT_SOURCE_DIR}/patches/disable-exiv2-install.patch)
FetchContent_MakeAvailable(exiv2)

# Compile Flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CXX_COMMON_WARNING_FLAGS
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
      -Wimplicit-fallthrough
      -Werror
      -fstack-protector-strong
      -D_FORTIFY_SOURCE=2
      -fPIE)

  if(CMAKE_C_LIB_MUSL)
    message(STATUS "Detected MUSL libc, removing -D_FORTIFY_SOURCE=2 from CXX_COMMON_WARNING_FLAGS")
    # Remove -D_FORTIFY_SOURCE=2 from the list
    list(REMOVE_ITEM CXX_COMMON_WARNING_FLAGS "-D_FORTIFY_SOURCE=2")
  endif()

  # For release builds
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    list(APPEND CXX_COMMON_WARNING_FLAGS -O2 -DNDEBUG)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
  endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CXX_COMMON_WARNING_FLAGS /W4 /WX /permissive- /guard:cf)

  # Release optimizations
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    list(APPEND CXX_COMMON_WARNING_FLAGS /O2 /Oi /Ot /GL)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG /OPT:REF /OPT:ICF")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /HIGHENTROPYVA /NXCOMPAT /DYNAMICBASE")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /LTCG /OPT:REF /OPT:ICF")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /HIGHENTROPYVA /NXCOMPAT /DYNAMICBASE")
  endif()
endif()

# Core implementation sources (no bindings)
set(CORE_SOURCES
    src/exifmwg/KeywordInfoModel.cpp src/exifmwg/XmpAreaStruct.cpp src/exifmwg/DimensionsStruct.cpp
    src/exifmwg/RegionInfoStruct.cpp src/exifmwg/XmpUtils.cpp src/exifmwg/ImageMetadata.cpp)

if(BUILD_TESTING)
  # Create a static library for testing (core sources only)
  add_library(exifmwg_test_lib STATIC ${CORE_SOURCES})

  # Add the same include directories as the main target
  target_include_directories(exifmwg_test_lib PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

  target_include_directories(exifmwg_test_lib PUBLIC src/exifmwg/)

  target_include_directories(exifmwg_test_lib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

  # Link against Exiv2 library
  target_link_libraries(exifmwg_test_lib PUBLIC Exiv2::exiv2lib)

  target_compile_options(exifmwg_test_lib PRIVATE ${CXX_COMMON_WARNING_FLAGS})

  # Add coverage flags if requested
  if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(exifmwg_test_lib PRIVATE --coverage)
    target_link_options(exifmwg_test_lib PRIVATE --coverage)
  endif()

  # Enable clang tidy as requested
  if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY clang-tidy REQUIRED)
    set_target_properties(exifmwg_test_lib PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY}")
  endif()

  # And add the actual tests
  add_subdirectory(tests/unit)

else()

  # Find Python
  find_package(
    Python
    COMPONENTS Interpreter Development.Module
    REQUIRED)

  # Find nanobind
  find_package(nanobind CONFIG REQUIRED)

  # Add the nanobind module
  nanobind_add_module(bindings NB_STATIC NB_SUPPRESS_WARNINGS LTO src/exifmwg/bindings.cpp ${CORE_SOURCES})

  # TODO: Investigate STABLE_ABI

  # Thank you, Windows
  set(ENV{PYTHONPATH} "${CMAKE_CURRENT_BINARY_DIR}:$ENV{PYTHONPATH}")

  nanobind_add_stub(
    bindings_stub
    MODULE
    bindings
    OUTPUT
    ${CMAKE_CURRENT_SOURCE_DIR}/src/exifmwg/bindings.pyi
    DEPENDS
    bindings
    MARKER_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/exifmwg/py.typed)

  # This is probably a bug in Exiv2 that this header goes here
  target_include_directories(bindings PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  target_include_directories(bindings PRIVATE ${EXPAT_INCLUDE_DIRS})

  target_include_directories(bindings PRIVATE src/exifmwg/)

  target_link_libraries(bindings PUBLIC Exiv2::exiv2lib)

  # Set properties
  target_compile_definitions(bindings PRIVATE VERSION_INFO="${SKBUILD_PROJECT_VERSION}" NANOBIND_MODULE)
  target_compile_options(bindings PRIVATE ${CXX_COMMON_WARNING_FLAGS})

  install(TARGETS bindings DESTINATION "${SKBUILD_PROJECT_NAME}")
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/exifmwg/bindings.pyi
                ${CMAKE_CURRENT_SOURCE_DIR}/src/exifmwg/py.typed DESTINATION "${SKBUILD_PROJECT_NAME}")

endif()
