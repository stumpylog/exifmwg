
#include <filesystem>
#include <fstream>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "Image.hpp"
#include "TestUtils.hpp"

TEST_CASE_METHOD(ImageTestFixture, "Image::clearMetadata removes various data", "[image][clearing]") {

  SECTION("removes EXIF orientation") {
    auto tempPath = getTempSample(SampleImage::Sample2);
    Image image(tempPath);
    REQUIRE(image.Orientation.has_value());

    image.clearMetadata();

    CHECK_FALSE(image.Orientation.has_value());
  }

  SECTION("removes titles and descriptions") {
    auto tempPath = getTempSample(SampleImage::Sample1);
    Image image(tempPath);
    REQUIRE(image.Description.has_value());

    image.clearMetadata();

    CHECK_FALSE(image.Title.has_value());
    CHECK_FALSE(image.Description.has_value());
  }

  SECTION("removes face regions (MWG-RS)") {
    auto tempPath = getTempSample(SampleImage::Sample1);
    Image image(tempPath);
    REQUIRE(image.RegionInfo.has_value());

    image.clearMetadata();

    CHECK_FALSE(image.RegionInfo.has_value());
  }

  // TODO: There is some odd behavior here.  Erased, but somehow also not
  // SECTION("removes all keyword-related metadata") {
  //   auto tempPath = getTempSample(SampleImage::Sample1);
  //   Image image(tempPath);
  //   REQUIRE(image.LastKeywordXMP.has_value());
  //   REQUIRE(image.HierarchicalSubject.has_value());

  //   image.clearMetadata();

  //   CHECK_FALSE(image.LastKeywordXMP.has_value());
  //   CHECK_FALSE(image.TagsList.has_value());
  //   CHECK_FALSE(image.CatalogSets.has_value());
  //   CHECK_FALSE(image.HierarchicalSubject.has_value());
  //   CHECK_FALSE(image.KeywordInfo.has_value());
  // }
}

TEST_CASE_METHOD(ImageTestFixture, "Image::clearMetadata handles multiple samples", "[image][clearing]") {
  SECTION("works with different image files") {
    std::vector<SampleImage> samplesToTest = {SampleImage::Sample1, SampleImage::Sample2,   SampleImage::Sample3,
                                              SampleImage::Sample4, SampleImage::SamplePNG, SampleImage::SampleWEBP};

    for (const auto& sample : samplesToTest) {
      auto tempPath = getTempSample(sample);
      Image image(tempPath);
      REQUIRE_NOTHROW(image.clearMetadata());
      CHECK(image.ImageWidth > 0);
      CHECK(image.ImageHeight > 0);
    }
  }
}

TEST_CASE_METHOD(ImageTestFixture, "Image::clearMetadata preserves essential image data", "[image][clearing]") {
  SECTION("preserves image dimensions") {
    auto tempPath = getTempSample(SampleImage::Sample1);
    Image image(tempPath);
    auto initialWidth = image.ImageWidth;
    auto initialHeight = image.ImageHeight;
    REQUIRE(initialWidth > 0);

    image.clearMetadata();

    CHECK(image.ImageWidth == initialWidth);
    CHECK(image.ImageHeight == initialHeight);
  }
}

TEST_CASE_METHOD(ImageTestFixture, "Image::clearMetadata is idempotent", "[image][clearing]") {
  SECTION("multiple calls produce same result") {
    auto tempPath = getTempSample(SampleImage::Sample1);
    Image image(tempPath);

    image.clearMetadata();
    auto titleAfterFirstClear = image.Title;
    auto descriptionAfterFirstClear = image.Description;
    auto orientationAfterFirstClear = image.Orientation;

    image.clearMetadata();

    CHECK(image.Title == titleAfterFirstClear);
    CHECK(image.Description == descriptionAfterFirstClear);
    CHECK(image.Orientation == orientationAfterFirstClear);
  }
}
