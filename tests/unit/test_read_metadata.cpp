#include <fstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "Image.hpp"
#include "TestUtils.hpp"

TEST_CASE_METHOD(ImageTestFixture, "Image constructor extracts complete metadata from sample1.jpg",
                 "[image][reading]") {
  REQUIRE(hasSample(SampleImage::Sample1));
  auto imagePath = getOriginalSample(SampleImage::Sample1);

  Image image(imagePath);

  SECTION("Basic image properties") {
    CHECK(image.ImageHeight == 683);
    CHECK(image.ImageWidth == 1024);
    CHECK(!image.Orientation.has_value());
  }

  SECTION("Title and Description") {
    CHECK(!image.Title.has_value());
    CHECK(image.Description.has_value());
    CHECK(image.Description.value() ==
          "President Barack Obama throws a ball for Bo, the family dog, in the Rose Garden of the White House, Sept. "
          "9, 2010.  (Official White House Photo by Pete Souza)");
  }

  SECTION("Location data") {
    CHECK(image.Country == "USA");
    CHECK(image.City == "WASHINGTON");
    CHECK(image.State == "DC");
    CHECK(!image.Location.has_value());
  }

  SECTION("Keywords and tags") {
    REQUIRE(image.LastKeywordXMP.has_value());
    auto& keywords = image.LastKeywordXMP.value();
    CHECK(keywords.size() == 4);
    CHECK(std::find(keywords.begin(), keywords.end(), "People/Barack Obama") != keywords.end());
    CHECK(std::find(keywords.begin(), keywords.end(), "Locations/United States/District of Columbia/Washington DC") !=
          keywords.end());

    REQUIRE(image.TagsList.has_value());
    CHECK(image.TagsList.value() == keywords);

    REQUIRE(image.CatalogSets.has_value());
    auto& catalogSets = image.CatalogSets.value();
    CHECK(catalogSets.size() == 4);
    CHECK(std::find(catalogSets.begin(), catalogSets.end(), "People|Barack Obama") != catalogSets.end());

    REQUIRE(image.HierarchicalSubject.has_value());
    CHECK(image.HierarchicalSubject.value() == catalogSets);
  }

  SECTION("Region Info") {
    REQUIRE(image.RegionInfo.has_value());
    auto& regionInfo = image.RegionInfo.value();

    CHECK_THAT(regionInfo.AppliedToDimensions.H, Catch::Matchers::WithinRel(683.0, 0.001));
    CHECK_THAT(regionInfo.AppliedToDimensions.W, Catch::Matchers::WithinRel(1024.0, 0.001));
    REQUIRE(regionInfo.RegionList.size() == 2);

    auto& obamaRegion = regionInfo.RegionList[0];
    CHECK(obamaRegion.Name == "Barack Obama");
    CHECK(obamaRegion.Type == "Face");
    CHECK_THAT(obamaRegion.Area.Y, Catch::Matchers::WithinRel(0.303075, 0.0001));

    auto& boRegion = regionInfo.RegionList[1];
    CHECK(boRegion.Name == "Bo");
    CHECK(boRegion.Type == "Pet");
    CHECK_THAT(boRegion.Area.Y, Catch::Matchers::WithinRel(0.768668, 0.0001));
  }
}

TEST_CASE_METHOD(ImageTestFixture, "Image constructor handles various sample files", "[image][reading]") {
  SECTION("Sample 2") {
    REQUIRE(hasSample(SampleImage::Sample2));
    Image image(getOriginalSample(SampleImage::Sample2));
    CHECK(image.ImageHeight == 2333);
    CHECK(image.ImageWidth == 3500);
    CHECK(image.Orientation == 1);
    REQUIRE(image.RegionInfo.has_value());
    CHECK(image.RegionInfo->RegionList.size() == 1);
    CHECK(image.RegionInfo->RegionList[0].Name == "Barack Obama");
  }

  SECTION("Sample 3") {
    REQUIRE(hasSample(SampleImage::Sample3));
    Image image(getOriginalSample(SampleImage::Sample3));
    CHECK(image.ImageHeight == 1000);
    CHECK(image.ImageWidth == 1500);
    REQUIRE(image.RegionInfo.has_value());
    CHECK(image.RegionInfo->RegionList.size() == 4);
  }

  SECTION("All supported formats") {
    std::vector<SampleImage> samplesToTest = {SampleImage::Sample1, SampleImage::Sample2,   SampleImage::Sample3,
                                              SampleImage::Sample4, SampleImage::SamplePNG, SampleImage::SampleWEBP};
    for (const auto& sample : samplesToTest) {
      auto tempPath = getOriginalSample(sample);
      REQUIRE_NOTHROW(Image(tempPath));
    }
  }
}

TEST_CASE_METHOD(ImageTestFixture, "Image constructor error handling", "[image][reading][error]") {
  SECTION("throws on missing file") {
    std::filesystem::path nonexistentPath = "nonexistent_image.jpg";
    CHECK_THROWS_AS(Image(nonexistentPath), std::runtime_error);
  }

  SECTION("throws on corrupted or non-image file") {
    auto tempPath = std::filesystem::temp_directory_path() / "corrupted_test.jpg";
    std::ofstream file(tempPath, std::ios::binary);
    file << "This is not a valid image file";
    file.close();

    CHECK_THROWS_AS(Image(tempPath), std::runtime_error);

    std::filesystem::remove(tempPath);
  }
}
