#include <catch2/catch_test_macros.hpp>

#include "Orientation.hpp"

TEST_CASE("ExifOrientation conversion functions", "[orientation]") {

  SECTION("orientation_to_int conversion") {
    // Test all valid enum values
    REQUIRE(orientation_to_int(ExifOrientation::Undefined) == 0);
    REQUIRE(orientation_to_int(ExifOrientation::Horizontal) == 1);
    REQUIRE(orientation_to_int(ExifOrientation::MirrorHorizontal) == 2);
    REQUIRE(orientation_to_int(ExifOrientation::Rotate180) == 3);
    REQUIRE(orientation_to_int(ExifOrientation::MirrorVertical) == 4);
    REQUIRE(orientation_to_int(ExifOrientation::MirrorHorizontalAndRotate270CW) == 5);
    REQUIRE(orientation_to_int(ExifOrientation::Rotate90CW) == 6);
    REQUIRE(orientation_to_int(ExifOrientation::MirrorHorizontalAndRotate90CW) == 7);
    REQUIRE(orientation_to_int(ExifOrientation::Rotate270CW) == 8);

    // Alias tests
    REQUIRE(orientation_to_int(ExifOrientation::TopLeft) == 1);
    REQUIRE(orientation_to_int(ExifOrientation::TopRight) == 2);
    REQUIRE(orientation_to_int(ExifOrientation::BottomRight) == 3);
    REQUIRE(orientation_to_int(ExifOrientation::BottomLeft) == 4);
    REQUIRE(orientation_to_int(ExifOrientation::LeftTop) == 5);
    REQUIRE(orientation_to_int(ExifOrientation::RightTop) == 6);
    REQUIRE(orientation_to_int(ExifOrientation::RightBottom) == 7);
    REQUIRE(orientation_to_int(ExifOrientation::LeftBottom) == 8);
  }

  SECTION("orientation_from_int conversion") {
    // Test valid integer inputs (branch: value >= 0 && value <= 8 is true)
    REQUIRE(orientation_from_int(0) == ExifOrientation::Undefined);
    REQUIRE(orientation_from_int(1) == ExifOrientation::Horizontal);
    REQUIRE(orientation_from_int(2) == ExifOrientation::MirrorHorizontal);
    REQUIRE(orientation_from_int(3) == ExifOrientation::Rotate180);
    REQUIRE(orientation_from_int(4) == ExifOrientation::MirrorVertical);
    REQUIRE(orientation_from_int(5) == ExifOrientation::MirrorHorizontalAndRotate270CW);
    REQUIRE(orientation_from_int(6) == ExifOrientation::Rotate90CW);
    REQUIRE(orientation_from_int(7) == ExifOrientation::MirrorHorizontalAndRotate90CW);
    REQUIRE(orientation_from_int(8) == ExifOrientation::Rotate270CW);

    // Test invalid integer inputs (branch: value >= 0 && value <= 8 is false)
    REQUIRE(orientation_from_int(-1) == ExifOrientation::Undefined);
    REQUIRE(orientation_from_int(9) == ExifOrientation::Undefined);
    REQUIRE(orientation_from_int(100) == ExifOrientation::Undefined);
  }

  SECTION("orientation_to_exif_value and orientation_from_exif_value aliases") {
    // These should behave identically to their underlying functions
    REQUIRE(orientation_to_exif_value(ExifOrientation::Rotate90CW) == 6);
    REQUIRE(orientation_from_exif_value(6) == ExifOrientation::Rotate90CW);
    REQUIRE(orientation_from_exif_value(99) == ExifOrientation::Undefined);
  }

  SECTION("orientation_is_valid check") {
    // Test valid orientations (branch: orientation != ExifOrientation::Undefined is true)
    REQUIRE(orientation_is_valid(ExifOrientation::Horizontal));
    REQUIRE(orientation_is_valid(ExifOrientation::Rotate90CW));
    REQUIRE(orientation_is_valid(ExifOrientation::MirrorVertical));

    // Test invalid orientation (branch: orientation != ExifOrientation::Undefined is false)
    REQUIRE_FALSE(orientation_is_valid(ExifOrientation::Undefined));
  }

  SECTION("orientation_to_string conversion") {
    // Test all defined enum values (hitting all switch cases)
    REQUIRE(std::string(orientation_to_string(ExifOrientation::Undefined)) == "Undefined");
    REQUIRE(std::string(orientation_to_string(ExifOrientation::Horizontal)) == "Horizontal");
    REQUIRE(std::string(orientation_to_string(ExifOrientation::MirrorHorizontal)) == "MirrorHorizontal");
    REQUIRE(std::string(orientation_to_string(ExifOrientation::Rotate180)) == "Rotate180");
    REQUIRE(std::string(orientation_to_string(ExifOrientation::MirrorVertical)) == "MirrorVertical");
    REQUIRE(std::string(orientation_to_string(ExifOrientation::MirrorHorizontalAndRotate270CW)) ==
            "MirrorHorizontalAndRotate270CW");
    REQUIRE(std::string(orientation_to_string(ExifOrientation::Rotate90CW)) == "Rotate90CW");
    REQUIRE(std::string(orientation_to_string(ExifOrientation::MirrorHorizontalAndRotate90CW)) ==
            "MirrorHorizontalAndRotate90CW");
    REQUIRE(std::string(orientation_to_string(ExifOrientation::Rotate270CW)) == "Rotate270CW");

    // Test an "unknown" value (to hit the default case in orientation_to_string)
    // This requires casting an int to ExifOrientation that is outside the defined enum values
    // This is primarily for branch coverage of the default case, acknowledging it's an edge case for the enum.
    REQUIRE(std::string(orientation_to_string(static_cast<ExifOrientation>(99))) == "Unknown");
  }
}
