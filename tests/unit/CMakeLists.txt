# Fetch Catch2
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.8.1)
FetchContent_MakeAvailable(Catch2)

# Test executable
add_executable(
  tests
  test_xmp_utils.cpp
  test_xmp_area.cpp
  test_dimensions_struct.cpp
  test_keyword_info.cpp
  test_region_info.cpp
  test_image_metadata.cpp
  test_read_metadata.cpp
  test_write_metadata.cpp
  test_clear_metadata.cpp)

# Link libraries
target_link_libraries(tests PRIVATE exifmwg_test_lib Catch2::Catch2WithMain)

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(exifmwg_test_lib PRIVATE --coverage)
  target_link_options(exifmwg_test_lib PRIVATE --coverage)

  target_compile_options(tests PRIVATE --coverage)
  target_link_options(tests PRIVATE --coverage)
endif()

# Include Catch2 CMake helper functions
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)

# Coverage report target
if(ENABLE_COVERAGE)
  # Find LCOV & genhtml
  find_program(LCOV_PATH lcov REQUIRED)
  find_program(GENHTML_PATH genhtml REQUIRED)

  add_custom_target(
    coverage-zero
    COMMENT "=== Zeroing coverage counters ==="
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ${LCOV_PATH} --zerocounters --directory ${CMAKE_BINARY_DIR})

  add_custom_target(
    coverage
    COMMENT "=== Generating code & branch coverage report ==="
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    # Capture coverage *with* branch data
    COMMAND
      ${LCOV_PATH} --capture --rc branch_coverage=1 --directory
      ${CMAKE_BINARY_DIR}/CMakeFiles/exifmwg_test_lib.dir --directory
      ${CMAKE_BINARY_DIR}/tests/unit/CMakeFiles/tests.dir --base-directory
      ${CMAKE_SOURCE_DIR} --output-file coverage.info --no-external
    # Remove other externals
    COMMAND ${LCOV_PATH} --remove coverage.info "catch2/*" "exiv2/*" "tests/*"
            --rc branch_coverage=1 --output-file coverage_filtered.info
    # Generate HTML *with* branch coverage panels
    COMMAND
      ${GENHTML_PATH} --branch-coverage coverage_filtered.info
      --output-directory coverage_html --title "exigmwg C++ Test Coverage")

  add_dependencies(coverage tests)

endif()
