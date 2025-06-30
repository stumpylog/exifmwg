#include <optional>
#include <string>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

#include <exiv2/exiv2.hpp>

#include "Errors.hpp"
#include "TestUtils.hpp"
#include "XmpAreaStruct.hpp"
#include "XmpUtils.hpp"

TEST_CASE("XmpAreaStruct Constructor", "[XmpAreaStruct]") {
  SECTION("All parameters provided") {
    XmpAreaStruct area(10.0, 20.0, 5.0, 15.0, "pixels", 30.0);
    REQUIRE(area.H == 10.0);
    REQUIRE(area.W == 20.0);
    REQUIRE(area.X == 5.0);
    REQUIRE(area.Y == 15.0);
    REQUIRE(area.Unit == "pixels");
    REQUIRE(area.D.has_value());
    REQUIRE(area.D.value() == 30.0);
  }

  SECTION("Optional 'd' parameter not provided") {
    XmpAreaStruct area(1.0, 2.0, 0.5, 0.5, "normalized", std::nullopt);
    REQUIRE(area.H == 1.0);
    REQUIRE(area.W == 2.0);
    REQUIRE(area.X == 0.5);
    REQUIRE(area.Y == 0.5);
    REQUIRE(area.Unit == "normalized");
    REQUIRE_FALSE(area.D.has_value());
  }

  SECTION("Zero values") {
    XmpAreaStruct area(0.0, 0.0, 0.0, 0.0, "meters", 0.0);
    REQUIRE(area.H == 0.0);
    REQUIRE(area.W == 0.0);
    REQUIRE(area.X == 0.0);
    REQUIRE(area.Y == 0.0);
    REQUIRE(area.Unit == "meters");
    REQUIRE(area.D.has_value());
    REQUIRE(area.D.value() == 0.0);
  }

  SECTION("Negative values (for coordinates, though usually positive)") {
    XmpAreaStruct area(100.0, 200.0, -10.0, -20.0, "centimeters", -5.0);
    REQUIRE(area.H == 100.0);
    REQUIRE(area.W == 200.0);
    REQUIRE(area.X == -10.0);
    REQUIRE(area.Y == -20.0);
    REQUIRE(area.Unit == "centimeters");
    REQUIRE(area.D.has_value());
    REQUIRE(area.D.value() == -5.0);
  }
}

TEST_CASE("XmpAreaStruct::toXmp and fromXmp roundtrip", "[XmpAreaStruct]") {
  const std::string baseKey = "Xmp.mwg-rs.Regions/RegionList[1]";

  SECTION("Full XmpAreaStruct roundtrip") {
    Exiv2::XmpData xmpData;
    XmpAreaStruct originalArea(0.12345, 0.67891, 0.05, 0.95, "normalized", 0.5);
    originalArea.toXmp(xmpData, baseKey);

    // Verify direct XMP data entries (checking precision for doubles)
    REQUIRE(xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:h")) != xmpData.end());
    REQUIRE(XmpUtils::doubleToStringWithPrecision(originalArea.H) == xmpData[baseKey + "/stArea:h"].toString());
    REQUIRE(XmpUtils::doubleToStringWithPrecision(originalArea.W) == xmpData[baseKey + "/stArea:w"].toString());
    REQUIRE(XmpUtils::doubleToStringWithPrecision(originalArea.X) == xmpData[baseKey + "/stArea:x"].toString());
    REQUIRE(XmpUtils::doubleToStringWithPrecision(originalArea.Y) == xmpData[baseKey + "/stArea:y"].toString());
    REQUIRE(xmpData[baseKey + "/stArea:unit"].toString() == originalArea.Unit);
    REQUIRE(xmpData[baseKey + "/stArea:d"].toString() == std::to_string(*originalArea.D));

    XmpAreaStruct readArea = XmpAreaStruct::fromXmp(xmpData, baseKey);

    REQUIRE(readArea.H == Catch::Approx(originalArea.H));
    REQUIRE(readArea.W == Catch::Approx(originalArea.W));
    REQUIRE(readArea.X == Catch::Approx(originalArea.X));
    REQUIRE(readArea.Y == Catch::Approx(originalArea.Y));
    REQUIRE(readArea.Unit == originalArea.Unit);
    REQUIRE(readArea.D.has_value());
    REQUIRE(readArea.D.value() == Catch::Approx(originalArea.D.value()));
  }

  SECTION("XmpAreaStruct without 'd' value roundtrip") {
    Exiv2::XmpData xmpData;
    XmpAreaStruct originalArea(100.0, 200.0, 10.0, 20.0, "pixels", std::nullopt);
    originalArea.toXmp(xmpData, baseKey);

    // Verify 'd' is not present
    REQUIRE(xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:d")) == xmpData.end());

    XmpAreaStruct readArea = XmpAreaStruct::fromXmp(xmpData, baseKey);

    REQUIRE(readArea.H == Catch::Approx(originalArea.H));
    REQUIRE(readArea.W == Catch::Approx(originalArea.W));
    REQUIRE(readArea.X == Catch::Approx(originalArea.X));
    REQUIRE(readArea.Y == Catch::Approx(originalArea.Y));
    REQUIRE(readArea.Unit == originalArea.Unit);
    REQUIRE_FALSE(readArea.D.has_value());
  }

  SECTION("fromXmp with missing optional fields") {
    Exiv2::XmpData xmpData;
    xmpData[baseKey + "/stArea:h"] = "0.5";
    xmpData[baseKey + "/stArea:w"] = "0.5";
    xmpData[baseKey + "/stArea:x"] = "0.25";
    xmpData[baseKey + "/stArea:y"] = "0.25";
    // Unit and D are missing

    XmpAreaStruct readArea = XmpAreaStruct::fromXmp(xmpData, baseKey);

    REQUIRE(readArea.H == Catch::Approx(0.5));
    REQUIRE(readArea.W == Catch::Approx(0.5));
    REQUIRE(readArea.X == Catch::Approx(0.25));
    REQUIRE(readArea.Y == Catch::Approx(0.25));
    REQUIRE(readArea.Unit == "normalized"); // Default unit
    REQUIRE_FALSE(readArea.D.has_value());  // Default d
  }

  SECTION("fromXmp with all fields missing") {
    Exiv2::XmpData xmpData;

    REQUIRE_THROWS_MATCHES(XmpAreaStruct::fromXmp(xmpData, baseKey), MissingFieldError,
                           Catch::Matchers::Message("No height found in xmp area struct"));
  }
  SECTION("fromXmp with only height") {
    Exiv2::XmpData xmpData;
    xmpData[baseKey + "/stArea:h"] = "0.5";

    REQUIRE_THROWS_MATCHES(XmpAreaStruct::fromXmp(xmpData, baseKey), MissingFieldError,
                           Catch::Matchers::Message("No width found in xmp area struct"));
  }
  SECTION("fromXmp with only height & width") {
    Exiv2::XmpData xmpData;
    xmpData[baseKey + "/stArea:h"] = "0.5";
    xmpData[baseKey + "/stArea:w"] = "0.6";

    REQUIRE_THROWS_MATCHES(XmpAreaStruct::fromXmp(xmpData, baseKey), MissingFieldError,
                           Catch::Matchers::Message("No x found in xmp area struct"));
  }
  SECTION("fromXmp with missing y") {
    Exiv2::XmpData xmpData;
    xmpData[baseKey + "/stArea:h"] = "0.5";
    xmpData[baseKey + "/stArea:w"] = "0.6";
    xmpData[baseKey + "/stArea:x"] = "0.25";

    REQUIRE_THROWS_MATCHES(XmpAreaStruct::fromXmp(xmpData, baseKey), MissingFieldError,
                           Catch::Matchers::Message("No y found in xmp area struct"));
  }
}

TEST_CASE("XmpAreaStruct Equality Operator", "[XmpAreaStruct]") {
  SECTION("Two identical structs") {
    XmpAreaStruct area1(1.0, 2.0, 3.0, 4.0, "unit1", 5.0);
    XmpAreaStruct area2(1.0, 2.0, 3.0, 4.0, "unit1", 5.0);
    REQUIRE(area1 == area2);
  }

  SECTION("Structs with different H") {
    XmpAreaStruct area1(1.0, 2.0, 3.0, 4.0, "unit1", 5.0);
    XmpAreaStruct area2(1.1, 2.0, 3.0, 4.0, "unit1", 5.0);
    REQUIRE_FALSE(area1 == area2);
  }

  SECTION("Structs with different optional D values") {
    XmpAreaStruct area1(1.0, 2.0, 3.0, 4.0, "unit1", 5.0);
    XmpAreaStruct area2(1.0, 2.0, 3.0, 4.0, "unit1", 5.1);
    REQUIRE_FALSE(area1 == area2);
  }

  SECTION("One struct with D, one without") {
    XmpAreaStruct area1(1.0, 2.0, 3.0, 4.0, "unit1", 5.0);
    XmpAreaStruct area2(1.0, 2.0, 3.0, 4.0, "unit1", std::nullopt);
    REQUIRE_FALSE(area1 == area2);
  }

  SECTION("Both structs without D") {
    XmpAreaStruct area1(1.0, 2.0, 3.0, 4.0, "unit1", std::nullopt);
    XmpAreaStruct area2(1.0, 2.0, 3.0, 4.0, "unit1", std::nullopt);
    REQUIRE(area1 == area2);
  }

  SECTION("Structs with different units") {
    XmpAreaStruct area1(1.0, 2.0, 3.0, 4.0, "unit1", 5.0);
    XmpAreaStruct area2(1.0, 2.0, 3.0, 4.0, "unit2", 5.0);
    REQUIRE_FALSE(area1 == area2);
  }
}
