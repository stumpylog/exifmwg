# Fetch Catch2
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.8.1)
FetchContent_MakeAvailable(Catch2)

# Test executable
add_executable(tests test_xmp_utils.cpp)

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
    coverage
    COMMENT "=== Generating code & branch coverage report ==="
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    # Reset counters
    COMMAND ${LCOV_PATH} --zerocounters --directory .
    # Run your tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    # Capture coverage *with* branch data
    COMMAND
      ${LCOV_PATH} --capture --rc lcov_branch_coverage=1 --directory .
      --base-directory ${CMAKE_SOURCE_DIR} --output-file coverage.info
      --no-external
    # Remove Catch2 cruft
    COMMAND ${LCOV_PATH} --remove coverage.info "catch2/*" --rc
            lcov_branch_coverage=1 --output-file coverage_filtered.info
    # Generate HTML *with* branch coverage panels
    COMMAND ${GENHTML_PATH} --branch-coverage coverage_filtered.info
            --output-directory coverage_html)

  add_dependencies(coverage tests)

  add_dependencies(coverage tests)
endif()
