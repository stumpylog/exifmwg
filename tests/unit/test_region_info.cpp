#include <sstream>

#include <catch2/catch_test_macros.hpp>

#include <exiv2/exiv2.hpp>

#include "RegionInfoStruct.hpp"
#include "TestUtils.hpp"

#include <iostream>

TEST_CASE("RegionStruct: round-trip with description") {
  XmpAreaStruct area{0.4, 0.3, 0.1, 0.2, "normalized"};
  RegionInfoStruct::RegionStruct original(area, "Alice", "Face", "Smiling");

  Exiv2::XmpData xmp;
  original.toXmp(xmp, "Xmp.mwg-rs.Regions/mwg-rs:RegionList[1]");

  auto result = RegionInfoStruct::RegionStruct::fromXmp(xmp, "Xmp.mwg-rs.Regions/mwg-rs:RegionList[1]");
  REQUIRE(result == original);
}

TEST_CASE("RegionStruct: round-trip without description") {
  XmpAreaStruct area{0.4, 0.3, 0.1, 0.2, "normalized"};
  RegionInfoStruct::RegionStruct original(area, "Bob", "Pet", std::nullopt);

  Exiv2::XmpData xmp;
  original.toXmp(xmp, "Xmp.mwg-rs.Regions/mwg-rs:RegionList[1]");

  auto result = RegionInfoStruct::RegionStruct::fromXmp(xmp, "Xmp.mwg-rs.Regions/mwg-rs:RegionList[1]");
  REQUIRE(result == original);
}

TEST_CASE("RegionInfoStruct: single region round-trip") {
  DimensionsStruct dims{1080, 1920, "pixel"};
  RegionInfoStruct::RegionStruct region({0.4, 0.3, 0.1, 0.2, "normalized"}, "Person", "Face", "Smiling");

  RegionInfoStruct input(dims, {region});
  Exiv2::XmpData xmp;
  input.toXmp(xmp);

  auto result = RegionInfoStruct::fromXmp(xmp);
  REQUIRE(result == input);
}

TEST_CASE("RegionInfoStruct: multiple regions round-trip") {
  DimensionsStruct dims{720, 1280, "pixel"};

  RegionInfoStruct::RegionStruct r1({0.4, 0.3, 0.1, 0.2, "normalized"}, "Alice", "Face", "Smiling");
  RegionInfoStruct::RegionStruct r2({0.5, 0.2, 0.3, 0.4, "normalized"}, "Dog", "Pet", std::nullopt);

  RegionInfoStruct input(dims, {r1, r2});
  Exiv2::XmpData xmp;
  input.toXmp(xmp);

  auto result = RegionInfoStruct::fromXmp(xmp);
  REQUIRE(result == input);
}

TEST_CASE("RegionInfoStruct: empty region list") {
  DimensionsStruct dims{600, 800, "pixel"};
  RegionInfoStruct input(dims, {});
  Exiv2::XmpData xmp;
  input.toXmp(xmp);

  auto result = RegionInfoStruct::fromXmp(xmp);
  REQUIRE(result.RegionList.empty());
  REQUIRE(result == input);
}

TEST_CASE("RegionStruct equality and inequality") {
  XmpAreaStruct area1{0.4, 0.3, 0.1, 0.2, "normalized"};
  XmpAreaStruct area2{0.5, 0.2, 0.3, 0.4, "normalized"};

  RegionInfoStruct::RegionStruct a(area1, "Alice", "Face", "Test");
  RegionInfoStruct::RegionStruct b(area1, "Alice", "Face", "Test");
  RegionInfoStruct::RegionStruct c(area2, "Alice", "Face", "Test");
  RegionInfoStruct::RegionStruct d(area1, "Bob", "Face", "Test");

  REQUIRE(a == b);
  REQUIRE_FALSE(a == c);
  REQUIRE_FALSE(a == d);
}

TEST_CASE("RegionInfoStruct equality and inequality") {
  DimensionsStruct dims1{100, 200, "pixel"};
  DimensionsStruct dims2{100, 200, "pixel"};

  RegionInfoStruct::RegionStruct r1({0.1, 0.1, 0.2, 0.2, "normalized"}, "A", "Face", {});
  RegionInfoStruct::RegionStruct r2({0.3, 0.3, 0.1, 0.1, "normalized"}, "B", "Pet", "Dog");

  RegionInfoStruct a(dims1, {r1, r2});
  RegionInfoStruct b(dims2, {r1, r2});
  RegionInfoStruct c(dims2, {r1});

  REQUIRE(a == b);
  REQUIRE_FALSE(a == c);
}

TEST_CASE("RegionInfoStruct: fromXmp terminates on missing region entry") {
  Exiv2::XmpData xmp;

  // Add dimensions
  xmp["Xmp.mwg-rs.Regions/mwg-rs:AppliedToDimensions/stDim:w"] = "100";
  xmp["Xmp.mwg-rs.Regions/mwg-rs:AppliedToDimensions/stDim:h"] = "100";
  xmp["Xmp.mwg-rs.Regions/mwg-rs:AppliedToDimensions/stDim:unit"] = "pixel";

  // Add one region
  xmp["Xmp.mwg-rs.Regions/mwg-rs:RegionList[1]/mwg-rs:Name"] = "Face1";
  xmp["Xmp.mwg-rs.Regions/mwg-rs:RegionList[1]/mwg-rs:Type"] = "Face";
  xmp["Xmp.mwg-rs.Regions/mwg-rs:RegionList[1]/mwg-rs:Area/stArea:x"] = "0.1";
  xmp["Xmp.mwg-rs.Regions/mwg-rs:RegionList[1]/mwg-rs:Area/stArea:y"] = "0.2";
  xmp["Xmp.mwg-rs.Regions/mwg-rs:RegionList[1]/mwg-rs:Area/stArea:w"] = "0.3";
  xmp["Xmp.mwg-rs.Regions/mwg-rs:RegionList[1]/mwg-rs:Area/stArea:h"] = "0.4";
  xmp["Xmp.mwg-rs.Regions/mwg-rs:RegionList[1]/mwg-rs:Area/stArea:unit"] = "normalized";

  // Missing RegionList[2], should stop here
  auto result = RegionInfoStruct::fromXmp(xmp);
  REQUIRE(result.RegionList.size() == 1);
}
