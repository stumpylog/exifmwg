#include <catch2/catch_test_macros.hpp>

#include "XmpUtils.hpp"

TEST_CASE("trim-whitespace", "xmp-utils") {
  SECTION("Removing basic string whitespace") {
    REQUIRE(XmpUtils::trimWhitespace("  test string  ") == "test string");
  }
  SECTION("Trimming a string with no whitespace") {
    REQUIRE(XmpUtils::trimWhitespace("string") == "string");
  }
  SECTION("Trimming a string with all whitespace") {
    REQUIRE(XmpUtils::trimWhitespace("\t  \n") == "");
  }
}
