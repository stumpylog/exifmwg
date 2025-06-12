#include <fstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "TestUtils.hpp"

#include "reading.hpp"

TEST_CASE_METHOD(ImageTestFixture, "read_metadata extracts complete metadata from sample1.jpg", "[metadata][reading]") {
  REQUIRE(hasSample(1)); // Ensure test file exists

  auto imagePath = getOriginalSample(1);
  auto metadata = read_metadata(imagePath);

  SECTION("Basic image properties") {
    CHECK(metadata.ImageHeight == 683);
    CHECK(metadata.ImageWidth == 1024);
    CHECK(!metadata.Orientation.has_value()); // null in JSON
  }

  SECTION("Title and Description") {
    CHECK(!metadata.Title.has_value()); // null in JSON
    CHECK(metadata.Description.has_value());
    CHECK(metadata.Description.value() ==
          "President Barack Obama throws a ball for Bo, the family dog, in the Rose Garden of the White House, Sept. "
          "9, 2010.  (Official White House Photo by Pete Souza)");
  }

  SECTION("Location data") {
    CHECK(metadata.Country == "USA");
    CHECK(metadata.City == "WASHINGTON");
    CHECK(metadata.State == "DC");
    CHECK(!metadata.Location.has_value()); // null in JSON
  }

  SECTION("Keywords and tags") {
    REQUIRE(metadata.LastKeywordXMP.has_value());
    auto& keywords = metadata.LastKeywordXMP.value();
    CHECK(keywords.size() == 4);
    CHECK(std::find(keywords.begin(), keywords.end(), "People/Barack Obama") != keywords.end());
    CHECK(std::find(keywords.begin(), keywords.end(), "Locations/United States/District of Columbia/Washington DC") !=
          keywords.end());
    CHECK(std::find(keywords.begin(), keywords.end(), "Dates/2010/09 - September/9") != keywords.end());
    CHECK(std::find(keywords.begin(), keywords.end(), "Pets/Dogs/Bo") != keywords.end());

    REQUIRE(metadata.TagsList.has_value());
    CHECK(metadata.TagsList.value() == keywords); // Should be identical

    REQUIRE(metadata.CatalogSets.has_value());
    auto& catalogSets = metadata.CatalogSets.value();
    CHECK(catalogSets.size() == 4);
    CHECK(std::find(catalogSets.begin(), catalogSets.end(), "People|Barack Obama") != catalogSets.end());
    CHECK(std::find(catalogSets.begin(), catalogSets.end(),
                    "Locations|United States|District of Columbia|Washington DC") != catalogSets.end());

    REQUIRE(metadata.HierarchicalSubject.has_value());
    CHECK(metadata.HierarchicalSubject.value() == catalogSets); // Should be identical to CatalogSets
  }

  SECTION("Region Info") {
    REQUIRE(metadata.RegionInfo.has_value());
    auto& regionInfo = metadata.RegionInfo.value();

    // Check applied dimensions
    CHECK_THAT(regionInfo.AppliedToDimensions.H, Catch::Matchers::WithinRel(683.0, 0.001));
    CHECK_THAT(regionInfo.AppliedToDimensions.W, Catch::Matchers::WithinRel(1024.0, 0.001));
    CHECK(regionInfo.AppliedToDimensions.Unit == "pixel");

    // Check region list
    REQUIRE(regionInfo.RegionList.size() == 2);

    // Barack Obama region
    auto& obamaRegion = regionInfo.RegionList[0];
    CHECK(obamaRegion.Name == "Barack Obama");
    CHECK(obamaRegion.Type == "Face");
    CHECK(!obamaRegion.Description.has_value());

    CHECK_THAT(obamaRegion.Area.H, Catch::Matchers::WithinRel(0.0585652, 0.0001));
    CHECK_THAT(obamaRegion.Area.W, Catch::Matchers::WithinRel(0.0292969, 0.0001));
    CHECK_THAT(obamaRegion.Area.X, Catch::Matchers::WithinRel(0.317383, 0.0001));
    CHECK_THAT(obamaRegion.Area.Y, Catch::Matchers::WithinRel(0.303075, 0.0001));
    CHECK(obamaRegion.Area.Unit == "normalized");
    CHECK(!obamaRegion.Area.D.has_value());

    // Bo region
    auto& boRegion = regionInfo.RegionList[1];
    CHECK(boRegion.Name == "Bo");
    CHECK(boRegion.Type == "Pet");
    CHECK(boRegion.Description.has_value());
    CHECK(boRegion.Description.value() == "Bo was a pet dog of the Obama family");

    CHECK_THAT(boRegion.Area.H, Catch::Matchers::WithinRel(0.284041, 0.0001));
    CHECK_THAT(boRegion.Area.W, Catch::Matchers::WithinRel(0.202148, 0.0001));
    CHECK_THAT(boRegion.Area.X, Catch::Matchers::WithinRel(0.616699, 0.0001));
    CHECK_THAT(boRegion.Area.Y, Catch::Matchers::WithinRel(0.768668, 0.0001));
  }

  SECTION("Keyword hierarchy") {
    REQUIRE(metadata.KeywordInfo.has_value());
    auto& keywordInfo = metadata.KeywordInfo.value();
    REQUIRE(keywordInfo.Hierarchy.size() == 4);

    // Check People hierarchy
    auto& peopleNode = keywordInfo.Hierarchy[0];
    CHECK(peopleNode.Keyword == "People");
    CHECK(!peopleNode.Applied.has_value());
    REQUIRE(peopleNode.Children.size() == 1);
    CHECK(peopleNode.Children[0].Keyword == "Barack Obama");
    CHECK(peopleNode.Children[0].Children.empty());

    // Check Locations hierarchy
    auto& locationsNode = keywordInfo.Hierarchy[1];
    CHECK(locationsNode.Keyword == "Locations");
    REQUIRE(locationsNode.Children.size() == 1);
    CHECK(locationsNode.Children[0].Keyword == "United States");
    REQUIRE(locationsNode.Children[0].Children.size() == 1);
    CHECK(locationsNode.Children[0].Children[0].Keyword == "District of Columbia");
    REQUIRE(locationsNode.Children[0].Children[0].Children.size() == 1);
    CHECK(locationsNode.Children[0].Children[0].Children[0].Keyword == "Washington DC");

    // Check Dates hierarchy
    auto& datesNode = keywordInfo.Hierarchy[2];
    CHECK(datesNode.Keyword == "Dates");
    REQUIRE(datesNode.Children.size() == 1);
    CHECK(datesNode.Children[0].Keyword == "2010");
    REQUIRE(datesNode.Children[0].Children.size() == 1);
    CHECK(datesNode.Children[0].Children[0].Keyword == "09 - September");
    REQUIRE(datesNode.Children[0].Children[0].Children.size() == 1);
    CHECK(datesNode.Children[0].Children[0].Children[0].Keyword == "9");

    // Check Pets hierarchy
    auto& petsNode = keywordInfo.Hierarchy[3];
    CHECK(petsNode.Keyword == "Pets");
    REQUIRE(petsNode.Children.size() == 1);
    CHECK(petsNode.Children[0].Keyword == "Dogs");
    REQUIRE(petsNode.Children[0].Children.size() == 1);
    CHECK(petsNode.Children[0].Children[0].Keyword == "Bo");
  }
}

TEST_CASE_METHOD(ImageTestFixture, "read_metadata extracts metadata from sample2.jpg", "[metadata][reading]") {
  REQUIRE(hasSample(2));
  auto metadata = read_metadata(getOriginalSample(2));

  CHECK(metadata.ImageHeight == 2333);
  CHECK(metadata.ImageWidth == 3500);
  CHECK(metadata.Orientation == 1);
  CHECK(!metadata.Title.has_value());
  CHECK(metadata.Description->find("President Barack Obama signs a letter") != std::string::npos);

  // Single face region
  REQUIRE(metadata.RegionInfo.has_value());
  CHECK(metadata.RegionInfo->RegionList.size() == 1);
  CHECK(metadata.RegionInfo->RegionList[0].Name == "Barack Obama");
  CHECK(metadata.RegionInfo->RegionList[0].Type == "Face");

  // Keywords
  REQUIRE(metadata.LastKeywordXMP.has_value());
  CHECK(metadata.LastKeywordXMP->size() == 2);
  CHECK(std::find(metadata.LastKeywordXMP->begin(), metadata.LastKeywordXMP->end(), "People/Barack Obama") !=
        metadata.LastKeywordXMP->end());
}

TEST_CASE_METHOD(ImageTestFixture, "read_metadata extracts metadata from sample3.jpg", "[metadata][reading]") {
  REQUIRE(hasSample(3));
  auto metadata = read_metadata(getOriginalSample(3));

  CHECK(metadata.ImageHeight == 1000);
  CHECK(metadata.ImageWidth == 1500);
  CHECK(metadata.Orientation == 1);
  CHECK(metadata.Description->find("Much has been made of this photograph") != std::string::npos);

  // Multiple face regions
  REQUIRE(metadata.RegionInfo.has_value());
  CHECK(metadata.RegionInfo->RegionList.size() == 4);

  std::vector<std::string> expectedNames = {"Barack Obama", "Denis McDonough", "Hillary Clinton", "Joseph R Biden"};
  for (const auto& region : metadata.RegionInfo->RegionList) {
    CHECK(std::find(expectedNames.begin(), expectedNames.end(), region.Name) != expectedNames.end());
    CHECK(region.Type == "Face");
  }

  // Keywords for multiple people
  REQUIRE(metadata.LastKeywordXMP.has_value());
  CHECK(metadata.LastKeywordXMP->size() == 5);
  CHECK(std::find(metadata.LastKeywordXMP->begin(), metadata.LastKeywordXMP->end(), "People/Hillary Clinton") !=
        metadata.LastKeywordXMP->end());
}

TEST_CASE_METHOD(ImageTestFixture, "read_metadata extracts metadata from sample4.jpg", "[metadata][reading]") {
  REQUIRE(hasSample(4));
  auto metadata = read_metadata(getOriginalSample(4));

  CHECK(metadata.ImageHeight == 436);
  CHECK(metadata.ImageWidth == 654);
  CHECK(!metadata.Orientation.has_value());
  CHECK(metadata.Description->find("CREATOR: gd-jpeg") != std::string::npos);

  // No location data for this sample
  CHECK(!metadata.Country.has_value());
  CHECK(!metadata.City.has_value());
  CHECK(!metadata.State.has_value());

  // Still has keywords
  REQUIRE(metadata.LastKeywordXMP.has_value());
  CHECK(metadata.LastKeywordXMP->size() == 3);
  CHECK(std::find(metadata.LastKeywordXMP->begin(), metadata.LastKeywordXMP->end(), "Pets/Dogs/Bo") !=
        metadata.LastKeywordXMP->end());
}

TEST_CASE_METHOD(ImageTestFixture, "read_metadata handles missing file", "[metadata][reading][error]") {
  std::filesystem::path nonexistentPath = "nonexistent_image.jpg";
  CHECK_THROWS_AS(read_metadata(nonexistentPath), std::runtime_error);
}

TEST_CASE_METHOD(ImageTestFixture, "read_metadata handles corrupted file", "[metadata][reading][error]") {
  auto tempPath = std::filesystem::temp_directory_path() / "corrupted_test.jpg";
  std::ofstream file(tempPath, std::ios::binary);
  file << "This is not a valid image file";
  file.close();

  CHECK_THROWS_AS(read_metadata(tempPath), std::runtime_error);

  std::filesystem::remove(tempPath);
}
