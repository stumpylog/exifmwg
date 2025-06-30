#include <fstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "TestUtils.hpp"

#include "Errors.hpp"
#include "ImageMetadata.hpp"

TEST_CASE_METHOD(ImageTestFixture, "read_metadata extracts complete metadata from sample1.jpg", "[metadata][reading]") {
  REQUIRE(hasSample(SampleImage::Sample1)); // Ensure test file exists

  auto imagePath = getOriginalSample(SampleImage::Sample1);
  ImageMetadata metadata(imagePath);

  SECTION("Basic image properties") {
    CHECK(metadata.ImageHeight == 683);
    CHECK(metadata.ImageWidth == 1024);
    CHECK_FALSE(metadata.Orientation.has_value()); // null in JSON
  }

  SECTION("Title and Description") {
    CHECK_FALSE(metadata.Title.has_value()); // null in JSON
    CHECK(metadata.Description.has_value());
    CHECK(metadata.Description.value() ==
          "President Barack Obama throws a ball for Bo, the family dog, in the Rose Garden of the White House, Sept. "
          "9, 2010.  (Official White House Photo by Pete Souza)");
  }

  SECTION("Location data") {
    CHECK(metadata.Country == "USA");
    CHECK(metadata.City == "WASHINGTON");
    CHECK(metadata.State == "DC");
    CHECK_FALSE(metadata.Location.has_value()); // null in JSON
  }

  SECTION("Keywords consolidated into KeywordInfo") {
    REQUIRE(metadata.KeywordInfo.has_value());
    auto& keywordInfo = metadata.KeywordInfo.value();

    // Verify the hierarchy structure contains the expected keywords
    // Based on the original test, we expect 4 hierarchical paths:
    // - People/Barack Obama
    // - Locations/United States/District of Columbia/Washington DC
    // - Dates/2010/09 - September/9
    // - Pets/Dogs/Bo

    CHECK(keywordInfo.Hierarchy.size() == 4);

    // Check that we can find each top-level keyword
    std::vector<std::string> expectedTopLevel = {"People", "Locations", "Dates", "Pets"};
    for (const auto& expected : expectedTopLevel) {
      bool found = false;
      for (const auto& node : keywordInfo.Hierarchy) {
        if (node.Keyword == expected) {
          found = true;
          break;
        }
      }
      CHECK(found);
    }
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
    CHECK_FALSE(obamaRegion.Description.has_value());

    CHECK_THAT(obamaRegion.Area.H, Catch::Matchers::WithinRel(0.0585652, 0.0001));
    CHECK_THAT(obamaRegion.Area.W, Catch::Matchers::WithinRel(0.0292969, 0.0001));
    CHECK_THAT(obamaRegion.Area.X, Catch::Matchers::WithinRel(0.317383, 0.0001));
    CHECK_THAT(obamaRegion.Area.Y, Catch::Matchers::WithinRel(0.303075, 0.0001));
    CHECK(obamaRegion.Area.Unit == "normalized");
    CHECK_FALSE(obamaRegion.Area.D.has_value());

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

  SECTION("Keyword hierarchy structure verification") {
    REQUIRE(metadata.KeywordInfo.has_value());
    auto& keywordInfo = metadata.KeywordInfo.value();
    REQUIRE(keywordInfo.Hierarchy.size() == 4);

    // Find and check People hierarchy
    auto peopleIt = std::find_if(keywordInfo.Hierarchy.begin(), keywordInfo.Hierarchy.end(),
                                 [](const auto& node) { return node.Keyword == "People"; });
    REQUIRE(peopleIt != keywordInfo.Hierarchy.end());
    CHECK_FALSE(peopleIt->Applied.has_value());
    REQUIRE(peopleIt->Children.size() == 1);
    CHECK(peopleIt->Children[0].Keyword == "Barack Obama");
    CHECK(peopleIt->Children[0].Children.empty());

    // Find and check Locations hierarchy
    auto locationsIt = std::find_if(keywordInfo.Hierarchy.begin(), keywordInfo.Hierarchy.end(),
                                    [](const auto& node) { return node.Keyword == "Locations"; });
    REQUIRE(locationsIt != keywordInfo.Hierarchy.end());
    REQUIRE(locationsIt->Children.size() == 1);
    CHECK(locationsIt->Children[0].Keyword == "United States");
    REQUIRE(locationsIt->Children[0].Children.size() == 1);
    CHECK(locationsIt->Children[0].Children[0].Keyword == "District of Columbia");
    REQUIRE(locationsIt->Children[0].Children[0].Children.size() == 1);
    CHECK(locationsIt->Children[0].Children[0].Children[0].Keyword == "Washington DC");

    // Find and check Dates hierarchy
    auto datesIt = std::find_if(keywordInfo.Hierarchy.begin(), keywordInfo.Hierarchy.end(),
                                [](const auto& node) { return node.Keyword == "Dates"; });
    REQUIRE(datesIt != keywordInfo.Hierarchy.end());
    REQUIRE(datesIt->Children.size() == 1);
    CHECK(datesIt->Children[0].Keyword == "2010");
    REQUIRE(datesIt->Children[0].Children.size() == 1);
    CHECK(datesIt->Children[0].Children[0].Keyword == "09 - September");
    REQUIRE(datesIt->Children[0].Children[0].Children.size() == 1);
    CHECK(datesIt->Children[0].Children[0].Children[0].Keyword == "9");

    // Find and check Pets hierarchy
    auto petsIt = std::find_if(keywordInfo.Hierarchy.begin(), keywordInfo.Hierarchy.end(),
                               [](const auto& node) { return node.Keyword == "Pets"; });
    REQUIRE(petsIt != keywordInfo.Hierarchy.end());
    REQUIRE(petsIt->Children.size() == 1);
    CHECK(petsIt->Children[0].Keyword == "Dogs");
    REQUIRE(petsIt->Children[0].Children.size() == 1);
    CHECK(petsIt->Children[0].Children[0].Keyword == "Bo");
  }
}

TEST_CASE_METHOD(ImageTestFixture, "read_metadata extracts metadata from sample2.jpg", "[metadata][reading]") {
  REQUIRE(hasSample(SampleImage::Sample2));

  ImageMetadata metadata(getOriginalSample(SampleImage::Sample2));

  CHECK(metadata.ImageHeight == 2333);
  CHECK(metadata.ImageWidth == 3500);
  CHECK(metadata.Orientation == ExifOrientation::Horizontal);
  CHECK_FALSE(metadata.Title.has_value());
  CHECK(metadata.Description->find("President Barack Obama signs a letter") != std::string::npos);

  // Single face region
  REQUIRE(metadata.RegionInfo.has_value());
  CHECK(metadata.RegionInfo->RegionList.size() == 1);
  CHECK(metadata.RegionInfo->RegionList[0].Name == "Barack Obama");
  CHECK(metadata.RegionInfo->RegionList[0].Type == "Face");

  // Keywords now in KeywordInfo
  REQUIRE(metadata.KeywordInfo.has_value());
  // Should have 2 hierarchical paths based on original test
  auto& keywordInfo = metadata.KeywordInfo.value();

  // Verify we have the expected keywords in the hierarchy
  bool foundPeopleObama = false;
  for (const auto& topLevel : keywordInfo.Hierarchy) {
    if (topLevel.Keyword == "People" && !topLevel.Children.empty()) {
      for (const auto& child : topLevel.Children) {
        if (child.Keyword == "Barack Obama") {
          foundPeopleObama = true;
          break;
        }
      }
    }
  }
  CHECK(foundPeopleObama);
}

TEST_CASE_METHOD(ImageTestFixture, "read_metadata extracts metadata from sample3.jpg", "[metadata][reading]") {
  REQUIRE(hasSample(SampleImage::Sample3));
  ImageMetadata metadata(getOriginalSample(SampleImage::Sample3));

  CHECK(metadata.ImageHeight == 1000);
  CHECK(metadata.ImageWidth == 1500);
  CHECK(metadata.Orientation == ExifOrientation::Horizontal);
  CHECK(metadata.Description->find("Much has been made of this photograph") != std::string::npos);

  // Multiple face regions
  REQUIRE(metadata.RegionInfo.has_value());
  CHECK(metadata.RegionInfo->RegionList.size() == 4);

  std::vector<std::string> expectedNames = {"Barack Obama", "Denis McDonough", "Hillary Clinton", "Joseph R Biden"};
  for (const auto& region : metadata.RegionInfo->RegionList) {
    CHECK(std::find(expectedNames.begin(), expectedNames.end(), region.Name) != expectedNames.end());
    CHECK(region.Type == "Face");
  }

  // Keywords for multiple people now in KeywordInfo
  REQUIRE(metadata.KeywordInfo.has_value());
  auto& keywordInfo = metadata.KeywordInfo.value();

  // Verify we have keywords for Hillary Clinton among others
  bool foundHillaryClinton = false;
  for (const auto& topLevel : keywordInfo.Hierarchy) {
    if (topLevel.Keyword == "People" && !topLevel.Children.empty()) {
      for (const auto& child : topLevel.Children) {
        if (child.Keyword == "Hillary Clinton") {
          foundHillaryClinton = true;
          break;
        }
      }
    }
  }
  CHECK(foundHillaryClinton);
}

TEST_CASE_METHOD(ImageTestFixture, "read_metadata extracts metadata from sample4.jpg", "[metadata][reading]") {
  REQUIRE(hasSample(SampleImage::Sample4));
  ImageMetadata metadata(getOriginalSample(SampleImage::Sample4));

  CHECK(metadata.ImageHeight == 436);
  CHECK(metadata.ImageWidth == 654);
  CHECK_FALSE(metadata.Orientation.has_value());
  CHECK(metadata.Description->find("CREATOR: gd-jpeg") != std::string::npos);

  // No location data for this sample
  CHECK_FALSE(metadata.Country.has_value());
  CHECK_FALSE(metadata.City.has_value());
  CHECK_FALSE(metadata.State.has_value());

  // Still has keywords, now in KeywordInfo
  REQUIRE(metadata.KeywordInfo.has_value());
  auto& keywordInfo = metadata.KeywordInfo.value();

  // Should have Bo keyword based on original test
  bool foundBo = false;
  for (const auto& topLevel : keywordInfo.Hierarchy) {
    if (topLevel.Keyword == "Pets" && !topLevel.Children.empty()) {
      for (const auto& dogs : topLevel.Children) {
        if (dogs.Keyword == "Dogs" && !dogs.Children.empty()) {
          for (const auto& dog : dogs.Children) {
            if (dog.Keyword == "Bo") {
              foundBo = true;
              break;
            }
          }
        }
      }
    }
  }
  CHECK(foundBo);
}

TEST_CASE_METHOD(ImageTestFixture, "read_metadata handles missing file", "[metadata][reading][error]") {
  std::filesystem::path nonexistentPath = "nonexistent_image.jpg";

  REQUIRE_FALSE(std::filesystem::exists(nonexistentPath));

  CHECK_THROWS_AS(ImageMetadata(nonexistentPath), FileAccessError);
}

TEST_CASE_METHOD(ImageTestFixture, "read_metadata handles corrupted file", "[metadata][reading][error]") {
  auto tempPath = std::filesystem::temp_directory_path() / "corrupted_test.jpg";
  std::ofstream file(tempPath, std::ios::binary);
  file << "This is not a valid image file";
  file.close();

  CHECK_THROWS_AS(ImageMetadata(tempPath), Exiv2Error);

  std::filesystem::remove(tempPath);
}

TEST_CASE_METHOD(ImageTestFixture, "read_metadata handles multiple file formats", "[metadata][reading]") {
  SECTION("works with different image files") {
    std::vector<SampleImage> samplesToTest = {SampleImage::Sample1, SampleImage::Sample2,   SampleImage::Sample3,
                                              SampleImage::Sample4, SampleImage::SamplePNG, SampleImage::SampleWEBP};

    for (const auto& sample : samplesToTest) {
      auto tempPath = getOriginalSample(sample);
      REQUIRE_NOTHROW(ImageMetadata(tempPath));
    }
  }
}
