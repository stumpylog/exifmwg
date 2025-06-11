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
    REQUIRE_FALSE(metadata.LastKeywordXMP.has_value());
    REQUIRE_FALSE(metadata.TagsList.has_value());
    REQUIRE_FALSE(metadata.CatalogSets.has_value());
    REQUIRE_FALSE(metadata.HierarchicalSubject.has_value());
    REQUIRE_FALSE(metadata.KeywordInfo.has_value());
    REQUIRE_FALSE(metadata.Country.has_value());
    REQUIRE_FALSE(metadata.City.has_value());
    REQUIRE_FALSE(metadata.State.has_value());
    REQUIRE_FALSE(metadata.Location.has_value());
  }

  SECTION("Constructor with all parameters") {
    RegionInfoStruct regionInfo = createTestRegionInfo();
    KeywordInfoModel keywordInfo = createTestKeywordInfo();
    std::vector<std::string> keywords = {"keyword1", "keyword2"};
    std::vector<std::string> tags = {"tag1", "tag2"};
    std::vector<std::string> catalogs = {"catalog1", "catalog2"};
    std::vector<std::string> subjects = {"subject1", "subject2"};

    ImageMetadata metadata(1920, 1080, "Test Title", "Test Description", regionInfo, 6, keywords, tags, catalogs,
                           subjects, keywordInfo, "United States", "New York", "NY", "Central Park");

    REQUIRE(metadata.ImageHeight == 1920);
    REQUIRE(metadata.ImageWidth == 1080);
    REQUIRE(metadata.Title == "Test Title");
    REQUIRE(metadata.Description == "Test Description");
    REQUIRE(metadata.RegionInfo.has_value());
    REQUIRE(metadata.Orientation == 6);
    REQUIRE(metadata.LastKeywordXMP == keywords);
    REQUIRE(metadata.TagsList == tags);
    REQUIRE(metadata.CatalogSets == catalogs);
    REQUIRE(metadata.HierarchicalSubject == subjects);
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
    ImageMetadata metadata(1920, 1080, "", "", std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                           std::nullopt, std::nullopt, "", "", "", "");

    REQUIRE(metadata.Title == "");
    REQUIRE(metadata.Description == "");
    REQUIRE(metadata.Country == "");
    REQUIRE(metadata.City == "");
    REQUIRE(metadata.State == "");
    REQUIRE(metadata.Location == "");
  }

  SECTION("Constructor with empty vectors") {
    std::vector<std::string> emptyVec;
    ImageMetadata metadata(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, emptyVec, emptyVec,
                           emptyVec, emptyVec);

    REQUIRE(metadata.LastKeywordXMP->empty());
    REQUIRE(metadata.TagsList->empty());
    REQUIRE(metadata.CatalogSets->empty());
    REQUIRE(metadata.HierarchicalSubject->empty());
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
    ImageMetadata metadata1(1920, 1080, std::nullopt, std::nullopt, std::nullopt, 1);
    ImageMetadata metadata2(1920, 1080, std::nullopt, std::nullopt, std::nullopt, 2);
    ImageMetadata metadata3(1920, 1080);

    REQUIRE_FALSE(metadata1 == metadata2);
    REQUIRE_FALSE(metadata1 == metadata3);
  }

  SECTION("Different vector contents should not be equal") {
    std::vector<std::string> vec1 = {"a", "b"};
    std::vector<std::string> vec2 = {"a", "c"};
    std::vector<std::string> vec3 = {"a", "b", "c"};

    ImageMetadata metadata1(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, vec1);
    ImageMetadata metadata2(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, vec2);
    ImageMetadata metadata3(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, vec3);

    REQUIRE_FALSE(metadata1 == metadata2);
    REQUIRE_FALSE(metadata1 == metadata3);
    REQUIRE_FALSE(metadata2 == metadata3);
  }

  SECTION("Same vector contents should be equal") {
    std::vector<std::string> vec1 = {"a", "b", "c"};
    std::vector<std::string> vec2 = {"a", "b", "c"};

    ImageMetadata metadata1(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, vec1);
    ImageMetadata metadata2(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, vec2);

    REQUIRE(metadata1 == metadata2);
  }

  SECTION("Location fields comparison") {
    ImageMetadata metadata1(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                            std::nullopt, std::nullopt, std::nullopt, std::nullopt, "US", "NYC", "NY", "Central Park");
    ImageMetadata metadata2(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                            std::nullopt, std::nullopt, std::nullopt, std::nullopt, "US", "NYC", "NY", "Central Park");
    ImageMetadata metadata3(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                            std::nullopt, std::nullopt, std::nullopt, std::nullopt, "US", "NYC", "NY", "Times Square");

    REQUIRE(metadata1 == metadata2);
    REQUIRE_FALSE(metadata1 == metadata3);
  }

  SECTION("Complex object with all fields populated") {
    RegionInfoStruct regionInfo = createTestRegionInfo();
    KeywordInfoModel keywordInfo = createTestKeywordInfo();
    std::vector<std::string> keywords = {"keyword1", "keyword2"};
    std::vector<std::string> tags = {"tag1", "tag2"};
    std::vector<std::string> catalogs = {"catalog1", "catalog2"};
    std::vector<std::string> subjects = {"subject1", "subject2"};

    ImageMetadata metadata1(1920, 1080, "Title", "Description", regionInfo, 1, keywords, tags, catalogs, subjects,
                            keywordInfo, "US", "NYC", "NY", "Central Park");

    ImageMetadata metadata2(1920, 1080, "Title", "Description", regionInfo, 1, keywords, tags, catalogs, subjects,
                            keywordInfo, "US", "NYC", "NY", "Central Park");

    REQUIRE(metadata1 == metadata2);
  }
}

TEST_CASE("ImageMetadata Edge Cases", "[ImageMetadata][EdgeCases]") {
  SECTION("Mixed nullopt and empty values") {
    std::vector<std::string> emptyVec;
    ImageMetadata metadata1(1920, 1080, "", std::nullopt, std::nullopt, std::nullopt, emptyVec, std::nullopt);
    ImageMetadata metadata2(1920, 1080, "", std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt);

    REQUIRE_FALSE(metadata1 == metadata2);
  }

  SECTION("Vector order matters") {
    std::vector<std::string> vec1 = {"a", "b"};
    std::vector<std::string> vec2 = {"b", "a"};

    ImageMetadata metadata1(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, vec1);
    ImageMetadata metadata2(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, vec2);

    REQUIRE_FALSE(metadata1 == metadata2);
  }

  SECTION("Whitespace in string fields") {
    ImageMetadata metadata1(1920, 1080, "Title", "Description");
    ImageMetadata metadata2(1920, 1080, " Title ", " Description ");

    REQUIRE_FALSE(metadata1 == metadata2);
  }

  SECTION("Case sensitivity") {
    ImageMetadata metadata1(1920, 1080, "Title", std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                            std::nullopt, std::nullopt, std::nullopt, "US");
    ImageMetadata metadata2(1920, 1080, "title", std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                            std::nullopt, std::nullopt, std::nullopt, "us");

    REQUIRE_FALSE(metadata1 == metadata2);
  }

  SECTION("Large vector comparisons") {
    std::vector<std::string> largeVec1, largeVec2;
    for (int i = 0; i < 1000; ++i) {
      largeVec1.push_back("item" + std::to_string(i));
      largeVec2.push_back("item" + std::to_string(i));
    }

    ImageMetadata metadata1(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, largeVec1);
    ImageMetadata metadata2(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, largeVec2);

    REQUIRE(metadata1 == metadata2);

    // Modify one element
    largeVec2[500] = "different";
    ImageMetadata metadata3(1920, 1080, std::nullopt, std::nullopt, std::nullopt, std::nullopt, largeVec2);

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
    std::vector<std::string> keywords = {"keyword1", "keyword2"};
    ImageMetadata original(1920, 1080, "Title", "Description", std::nullopt, 6, keywords, std::nullopt, std::nullopt,
                           std::nullopt, std::nullopt, "US", "NYC", "NY", "Central Park");

    ImageMetadata copied(original);

    REQUIRE(original == copied);
    REQUIRE(copied.ImageHeight == 1920);
    REQUIRE(copied.ImageWidth == 1080);
    REQUIRE(copied.Title == "Title");
    REQUIRE(copied.Description == "Description");
    REQUIRE(copied.Orientation == 6);
    REQUIRE(copied.LastKeywordXMP == keywords);
    REQUIRE(copied.Country == "US");
    REQUIRE(copied.City == "NYC");
    REQUIRE(copied.State == "NY");
    REQUIRE(copied.Location == "Central Park");
  }
}
