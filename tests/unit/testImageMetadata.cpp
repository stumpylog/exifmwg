#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "DimensionsStruct.hpp"
#include "ImageMetadata.hpp"
#include "KeywordInfoModel.hpp"
#include "RegionInfoStruct.hpp"
#include "XmpAreaStruct.hpp"

// Helper function to create test RegionInfoStruct
RegionInfoStruct createTestRegionInfo() {
  DimensionsStruct dims(1920.0, 1080.0, "pixel");
  XmpAreaStruct area(0.1, 0.2, 0.3, 0.4, "normalized");
  RegionInfoStruct::RegionStruct region(area, "TestRegion", "Face", "Test description");
  return RegionInfoStruct(dims, {region});
}

// Helper function to create test KeywordInfoModel
KeywordInfoModel createTestKeywordInfo() {
  KeywordInfoModel::KeywordStruct child1("child1");
  KeywordInfoModel::KeywordStruct child2("child2");
  KeywordInfoModel::KeywordStruct parent("parent", {child1, child2});
  return KeywordInfoModel({parent});
}

TEST_CASE("ImageMetadata Constructor Tests", "[ImageMetadata][Constructor]") {
  SECTION("Minimal constructor with only required parameters") {
    ImageMetadata metadata(1920, 1080);

    REQUIRE(metadata.ImageHeight == 1920);
    REQUIRE(metadata.ImageWidth == 1080);
    REQUIRE_FALSE(metadata.Title.has_value());
    REQUIRE_FALSE(metadata.Description.has_value());
    REQUIRE_FALSE(metadata.RegionInfo.has_value());
    REQUIRE_FALSE(metadata.Orientation.has_value());
    REQUIRE_FALSE(metadata.KeywordInfo.has_value());
    REQUIRE_FALSE(metadata.Country.has_value());
    REQUIRE_FALSE(metadata.City.has_value());
    REQUIRE_FALSE(metadata.State.has_value());
    REQUIRE_FALSE(metadata.Location.has_value());
  }

  SECTION("Constructor with all parameters") {
    RegionInfoStruct regionInfo = createTestRegionInfo();
    KeywordInfoModel keywordInfo = createTestKeywordInfo();

    ImageMetadata metadata(1920, 1080, "Test Title", "Test Description", regionInfo, ExifOrientation::Rotate90CW,
                           keywordInfo, "United States", "New York", "NY", "Central Park");

    REQUIRE(metadata.ImageHeight == 1920);
    REQUIRE(metadata.ImageWidth == 1080);
    REQUIRE(metadata.Title == "Test Title");
    REQUIRE(metadata.Description == "Test Description");
    REQUIRE(metadata.RegionInfo.has_value());
    REQUIRE(metadata.Orientation == ExifOrientation::Rotate90CW);
    REQUIRE(metadata.KeywordInfo.has_value());
    REQUIRE(metadata.Country == "United States");
    REQUIRE(metadata.City == "New York");
    REQUIRE(metadata.State == "NY");
    REQUIRE(metadata.Location == "Central Park");
  }

  SECTION("Constructor with edge case dimensions") {
    ImageMetadata metadata1(0, 0);
    REQUIRE(metadata1.ImageHeight == 0);
    REQUIRE(metadata1.ImageWidth == 0);

    ImageMetadata metadata2(-1, -1);
    REQUIRE(metadata2.ImageHeight == -1);
    REQUIRE(metadata2.ImageWidth == -1);

    ImageMetadata metadata3(INT_MAX, INT_MAX);
    REQUIRE(metadata3.ImageHeight == INT_MAX);
    REQUIRE(metadata3.ImageWidth == INT_MAX);
  }

  SECTION("Constructor with empty string values") {
    ImageMetadata metadata(1920, 1080, "", "", std::nullopt, std::nullopt, std::nullopt, "", "", "", "");

    REQUIRE(metadata.Title == "");
    REQUIRE(metadata.Description == "");
    REQUIRE(metadata.Country == "");
    REQUIRE(metadata.City == "");
    REQUIRE(metadata.State == "");
    REQUIRE(metadata.Location == "");
  }
}

TEST_CASE("ImageMetadata Equality Operator Tests", "[ImageMetadata][Equality]") {
  SECTION("Identical minimal objects should be equal") {
    ImageMetadata metadata1(1920, 1080);
    ImageMetadata metadata2(1920, 1080);

    REQUIRE(metadata1 == metadata2);
    REQUIRE_FALSE(metadata1 != metadata2);
  }

  SECTION("Different dimensions should not be equal") {
    ImageMetadata metadata1(1920, 1080);
    ImageMetadata metadata2(1920, 1081);
    ImageMetadata metadata3(1921, 1080);

    REQUIRE_FALSE(metadata1 == metadata2);
    REQUIRE(metadata1 != metadata2);
    REQUIRE_FALSE(metadata1 == metadata3);
    REQUIRE(metadata1 != metadata3);
  }

  SECTION("Different optional string fields should not be equal") {
    ImageMetadata metadata1(1920, 1080, "Title1");
    ImageMetadata metadata2(1920, 1080, "Title2");
    ImageMetadata metadata3(1920, 1080);

    REQUIRE_FALSE(metadata1 == metadata2);
    REQUIRE_FALSE(metadata1 == metadata3);
    REQUIRE_FALSE(metadata2 == metadata3);
  }

  SECTION("Different orientation values should not be equal") {
    ImageMetadata metadata1(1920, 1080, std::nullopt, std::nullopt, std::nullopt, ExifOrientation::Horizontal);
    ImageMetadata metadata2(1920, 1080, std::nullopt, std::nullopt, std::nullopt, ExifOrientation::MirrorHorizontal);
    ImageMetadata metadata3(1920, 1080);

    REQUIRE_FALSE(metadata1 == metadata2);
    REQUIRE_FALSE(metadata1 == metadata3);
  }

  SECTION("Different KeywordInfo should not be equal") {
    KeywordInfoModel keywordInfo1 = createTestKeywordInfo();
    KeywordInfoModel keywordInfo2({KeywordInfoModel::KeywordStruct("different")});

    ImageMetadata metadata1(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, keywordInfo1);
    ImageMetadata metadata2(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, keywordInfo2);
    ImageMetadata metadata3(1920, 1080);

    REQUIRE_FALSE(metadata1 == metadata2);
    REQUIRE_FALSE(metadata1 == metadata3);
    REQUIRE_FALSE(metadata2 == metadata3);
  }

  SECTION("Same KeywordInfo should be equal") {
    KeywordInfoModel keywordInfo1 = createTestKeywordInfo();
    KeywordInfoModel keywordInfo2 = createTestKeywordInfo();

    ImageMetadata metadata1(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, keywordInfo1);
    ImageMetadata metadata2(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, keywordInfo2);

    REQUIRE(metadata1 == metadata2);
  }

  SECTION("Location fields comparison") {
    ImageMetadata metadata1(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, "US",
                            "NYC", "NY", "Central Park");
    ImageMetadata metadata2(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, "US",
                            "NYC", "NY", "Central Park");
    ImageMetadata metadata3(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, "US",
                            "NYC", "NY", "Times Square");

    REQUIRE(metadata1 == metadata2);
    REQUIRE_FALSE(metadata1 == metadata3);
  }

  SECTION("Complex object with all fields populated") {
    RegionInfoStruct regionInfo = createTestRegionInfo();
    KeywordInfoModel keywordInfo = createTestKeywordInfo();

    ImageMetadata metadata1(1920, 1080, "Title", "Description", regionInfo, ExifOrientation::Horizontal, keywordInfo,
                            "US", "NYC", "NY", "Central Park");

    ImageMetadata metadata2(1920, 1080, "Title", "Description", regionInfo, ExifOrientation::Horizontal, keywordInfo,
                            "US", "NYC", "NY", "Central Park");

    REQUIRE(metadata1 == metadata2);
  }
}

TEST_CASE("ImageMetadata Edge Cases", "[ImageMetadata][EdgeCases]") {
  SECTION("Mixed nullopt and empty KeywordInfo") {
    KeywordInfoModel emptyKeywordInfo({}, '/');
    ImageMetadata metadata1(1920, 1080, "", std::nullopt, std::nullopt, std::nullopt, emptyKeywordInfo);
    ImageMetadata metadata2(1920, 1080, "", std::nullopt, std::nullopt, std::nullopt, std::nullopt);

    REQUIRE_FALSE(metadata1 == metadata2);
  }

  SECTION("Whitespace in string fields") {
    ImageMetadata metadata1(1920, 1080, "Title", "Description");
    ImageMetadata metadata2(1920, 1080, " Title ", " Description ");

    REQUIRE_FALSE(metadata1 == metadata2);
  }

  SECTION("Case sensitivity") {
    ImageMetadata metadata1(1920, 1080, "Title", std::nullopt, std::nullopt, std::nullopt, std::nullopt, "US");
    ImageMetadata metadata2(1920, 1080, "title", std::nullopt, std::nullopt, std::nullopt, std::nullopt, "us");

    REQUIRE_FALSE(metadata1 == metadata2);
  }

  SECTION("Large KeywordInfo hierarchies") {
    std::vector<KeywordInfoModel::KeywordStruct> largeHierarchy1, largeHierarchy2;
    for (int i = 0; i < 100; ++i) {
      largeHierarchy1.emplace_back("keyword" + std::to_string(i));
      largeHierarchy2.emplace_back("keyword" + std::to_string(i));
    }

    KeywordInfoModel keywordInfo1(largeHierarchy1);
    KeywordInfoModel keywordInfo2(largeHierarchy2);

    ImageMetadata metadata1(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, keywordInfo1);
    ImageMetadata metadata2(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, keywordInfo2);

    REQUIRE(metadata1 == metadata2);

    // Modify one element
    largeHierarchy2[50] = KeywordInfoModel::KeywordStruct("different");
    KeywordInfoModel keywordInfo3(largeHierarchy2);
    ImageMetadata metadata3(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, keywordInfo3);

    REQUIRE_FALSE(metadata1 == metadata3);
  }
}

TEST_CASE("ImageMetadata Inequality Operator", "[ImageMetadata][Inequality]") {
  SECTION("Inequality operator consistency") {
    ImageMetadata metadata1(1920, 1080, "Title1");
    ImageMetadata metadata2(1920, 1080, "Title2");
    ImageMetadata metadata3(1920, 1080, "Title1");

    // Test that != is consistent with ==
    REQUIRE((metadata1 == metadata2) == !(metadata1 != metadata2));
    REQUIRE((metadata1 == metadata3) == !(metadata1 != metadata3));
    REQUIRE((metadata2 == metadata3) == !(metadata2 != metadata3));
  }
}

TEST_CASE("ImageMetadata Copy and Assignment", "[ImageMetadata][Copy]") {
  SECTION("Copy constructor preserves all fields") {
    KeywordInfoModel keywordInfo = createTestKeywordInfo();
    ImageMetadata original(1920, 1080, "Title", "Description", std::nullopt, ExifOrientation::Rotate90CW, keywordInfo,
                           "US", "NYC", "NY", "Central Park");

    ImageMetadata copied(original);

    REQUIRE(original == copied);
    REQUIRE(copied.ImageHeight == 1920);
    REQUIRE(copied.ImageWidth == 1080);
    REQUIRE(copied.Title == "Title");
    REQUIRE(copied.Description == "Description");
    REQUIRE(copied.Orientation == ExifOrientation::Rotate90CW);
    REQUIRE(copied.KeywordInfo.has_value());
    REQUIRE(copied.Country == "US");
    REQUIRE(copied.City == "NYC");
    REQUIRE(copied.State == "NY");
    REQUIRE(copied.Location == "Central Park");
  }
}

TEST_CASE("ImageMetadata to_string", "[ImageMetadata][String]") {
  SECTION("Basic call succeeds") {
    KeywordInfoModel keywordInfo = createTestKeywordInfo();
    ImageMetadata original(1920, 1080, "Title", "Description", std::nullopt, ExifOrientation::Rotate90CW, keywordInfo,
                           "US", "NYC", "NY", "Central Park");
    REQUIRE_NOTHROW(original.to_string());
  }
}
