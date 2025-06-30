#include <stdexcept>
#include <string>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

#include <exiv2/exiv2.hpp>

#include "DimensionsStruct.hpp"
#include "Errors.hpp"
#include "TestUtils.hpp"
#include "XmpUtils.hpp"

TEST_CASE("DimensionsStruct Constructor", "[DimensionsStruct]") {
  SECTION("Typical positive dimensions") {
    DimensionsStruct dims(1920.0, 1080.0, "pixels");
    REQUIRE(dims.H == 1920.0);
    REQUIRE(dims.W == 1080.0);
    REQUIRE(dims.Unit == "pixels");
  }

  SECTION("Zero dimensions") {
    DimensionsStruct dims(0.0, 0.0, "centimeters");
    REQUIRE(dims.H == 0.0);
    REQUIRE(dims.W == 0.0);
    REQUIRE(dims.Unit == "centimeters");
  }

  SECTION("Floating point dimensions") {
    DimensionsStruct dims(1.23, 4.56, "inches");
    REQUIRE(dims.H == 1.23);
    REQUIRE(dims.W == 4.56);
    REQUIRE(dims.Unit == "inches");
  }

  SECTION("Empty unit string") {
    DimensionsStruct dims(100.0, 200.0, "");
    REQUIRE(dims.H == 100.0);
    REQUIRE(dims.W == 200.0);
    REQUIRE(dims.Unit == "");
  }
}

TEST_CASE("DimensionsStruct::toXmp and fromXmp roundtrip", "[DimensionsStruct]") {
  // Using a valid XMP path for dimensions
  const std::string baseKey = "Xmp.mwg-rs.Regions/RegionList[1]/stArea:dimensions";

  SECTION("Full DimensionsStruct roundtrip") {
    Exiv2::XmpData xmpData;
    DimensionsStruct originalDims(1920.0, 1080.0, "pixels");
    originalDims.toXmp(xmpData, baseKey);

    // Verify direct XMP data entries
    REQUIRE(xmpData.findKey(Exiv2::XmpKey(baseKey + "/stDim:h")) != xmpData.end());
    REQUIRE(XmpUtils::doubleToStringWithPrecision(originalDims.H) == xmpData[baseKey + "/stDim:h"].toString());
    REQUIRE(XmpUtils::doubleToStringWithPrecision(originalDims.W) == xmpData[baseKey + "/stDim:w"].toString());
    REQUIRE(xmpData[baseKey + "/stDim:unit"].toString() == originalDims.Unit);

    DimensionsStruct readDims = DimensionsStruct::fromXmp(xmpData, baseKey);

    REQUIRE((readDims.H, originalDims.H));
    REQUIRE((readDims.W, originalDims.W));
    REQUIRE(readDims.Unit == originalDims.Unit);
  }

  SECTION("Roundtrip with floating point values and non-default unit") {
    Exiv2::XmpData xmpData;
    DimensionsStruct originalDims(2.54, 1.0, "centimeters");
    originalDims.toXmp(xmpData, baseKey);

    DimensionsStruct readDims = DimensionsStruct::fromXmp(xmpData, baseKey);

    REQUIRE((readDims.H, originalDims.H));
    REQUIRE((readDims.W, originalDims.W));
    REQUIRE(readDims.Unit == originalDims.Unit);
  }

  SECTION("fromXmp with missing height (H) field") {
    Exiv2::XmpData xmpData;
    xmpData[baseKey + "/stDim:w"] = "100.0";
    xmpData[baseKey + "/stDim:unit"] = "pixels";

    REQUIRE_THROWS_MATCHES(DimensionsStruct::fromXmp(xmpData, baseKey), MissingFieldError,
                           Catch::Matchers::Message("No height found in dimensions struct"));
  }

  SECTION("fromXmp with missing width (W) field") {
    Exiv2::XmpData xmpData;
    xmpData[baseKey + "/stDim:h"] = "100.0";
    xmpData[baseKey + "/stDim:unit"] = "pixels";

    REQUIRE_THROWS_MATCHES(DimensionsStruct::fromXmp(xmpData, baseKey), MissingFieldError,
                           Catch::Matchers::Message("No width found in dimensions struct"));
  }

  SECTION("fromXmp with missing unit field") {
    Exiv2::XmpData xmpData;
    xmpData[baseKey + "/stDim:h"] = "100.0";
    xmpData[baseKey + "/stDim:w"] = "200.0";

    REQUIRE_THROWS_MATCHES(DimensionsStruct::fromXmp(xmpData, baseKey), MissingFieldError,
                           Catch::Matchers::Message("No unit found in dimensions struct"));
  }

  SECTION("fromXmp with empty XmpData") {
    Exiv2::XmpData xmpData; // Empty XmpData

    REQUIRE_THROWS_MATCHES(DimensionsStruct::fromXmp(xmpData, baseKey), MissingFieldError,
                           Catch::Matchers::Message("No height found in dimensions struct"));
  }

  SECTION("toXmp writes correct precision for doubles") {
    Exiv2::XmpData xmpData;
    DimensionsStruct dims(123.456789, 987.654321, "mm");
    dims.toXmp(xmpData, baseKey);

    REQUIRE(xmpData[baseKey + "/stDim:h"].toString() == "123.456789");
    REQUIRE(xmpData[baseKey + "/stDim:w"].toString() == "987.654321");
  }
}

TEST_CASE("DimensionsStruct Equality Operator", "[DimensionsStruct]") {
  SECTION("Two identical structs") {
    DimensionsStruct dims1(100.0, 200.0, "pixels");
    DimensionsStruct dims2(100.0, 200.0, "pixels");
    REQUIRE(dims1 == dims2);
  }

  SECTION("Structs with different H") {
    DimensionsStruct dims1(100.0, 200.0, "pixels");
    DimensionsStruct dims2(101.0, 200.0, "pixels");
    REQUIRE_FALSE(dims1 == dims2);
  }

  SECTION("Structs with different W") {
    DimensionsStruct dims1(100.0, 200.0, "pixels");
    DimensionsStruct dims2(100.0, 201.0, "pixels");
    REQUIRE_FALSE(dims1 == dims2);
  }

  SECTION("Structs with different Unit") {
    DimensionsStruct dims1(100.0, 200.0, "pixels");
    DimensionsStruct dims2(100.0, 200.0, "inches");
    REQUIRE_FALSE(dims1 == dims2);
  }

  SECTION("Structs with all different values") {
    DimensionsStruct dims1(100.0, 200.0, "pixels");
    DimensionsStruct dims2(1.0, 2.0, "cm");
    REQUIRE_FALSE(dims1 == dims2);
  }

  SECTION("Zero vs non-zero values") {
    DimensionsStruct dims1(0.0, 0.0, "pixels");
    DimensionsStruct dims2(1.0, 0.0, "pixels");
    REQUIRE_FALSE(dims1 == dims2);
  }
}
