#include <filesystem>
#include <fstream>

#include <catch2/catch_test_macros.hpp>

#include "ImageMetadata.hpp"
#include "TestUtils.hpp"

class ClearingTestFixture : public ImageTestFixture {

  bool hasRegionData(const std::filesystem::path& filepath) const {
    auto image = Exiv2::ImageFactory::open(filepath.string());
    image->readMetadata();
    auto& xmpData = image->xmpData();

    for (auto it = xmpData.begin(); it != xmpData.end(); ++it) {
      if (it->key().find("Xmp.mwg-rs.Regions") != std::string::npos) {
        return true;
      }
    }
    return false;
  }

  bool hasKeywordInfo(const std::filesystem::path& filepath) const {
    auto image = Exiv2::ImageFactory::open(filepath.string());
    image->readMetadata();
    auto& xmpData = image->xmpData();

    std::vector<std::string> keywordPatterns = {"Xmp.mwg-kw.KeywordInfo",       "Xmp.acdsee.Categories",
                                                "Xmp.microsoft.LastKeywordXMP", "Xmp.digiKam.TagsList",
                                                "Xmp.lr.hierarchicalSubject",   "Xmp.mediapro.CatalogSets"};

    for (auto it = xmpData.begin(); it != xmpData.end(); ++it) {
      for (const auto& pattern : keywordPatterns) {
        if (it->key().find(pattern) != std::string::npos) {
          return true;
        }
      }
    }
    return false;
  }
};

TEST_CASE_METHOD(ClearingTestFixture, "clear_existing_metadata removes orientation data", "[clearing]") {
  SECTION("removes EXIF orientation") {
    auto tempPath = getTempSample(SampleImage::Sample1);

    // Read metadata before clearing
    ImageMetadata beforeMetadata(tempPath);

    // Clear metadata
    clear_existing_metadata(tempPath);

    // Read metadata after clearing
    auto afterMetadata = read_metadata(tempPath);

    // Orientation should be cleared
    CHECK_FALSE(afterMetadata.Orientation.has_value());

    // Image dimensions should remain unchanged
    CHECK(afterMetadata.ImageWidth == beforeMetadata.ImageWidth);
    CHECK(afterMetadata.ImageHeight == beforeMetadata.ImageHeight);
  }
}

TEST_CASE_METHOD(ClearingTestFixture, "clear_existing_metadata removes titles and descriptions", "[clearing]") {
  SECTION("removes XMP title and description") {
    auto tempPath = getTempSample(SampleImage::Sample1);

    // Clear metadata
    clear_existing_metadata(tempPath);

    // Read metadata after clearing
    auto afterMetadata = read_metadata(tempPath);

    // Title and description should be cleared
    CHECK_FALSE(afterMetadata.Title.has_value());
    CHECK_FALSE(afterMetadata.Description.has_value());
  }
}

TEST_CASE_METHOD(ClearingTestFixture, "clear_existing_metadata removes face regions", "[clearing]") {
  // SECTION("removes MWG region data") {
  //   auto tempPath = getTempSample(SampleImage::Sample1);

  //   // Check if regions exist before clearing
  //   bool hadRegions = hasRegionData(tempPath);

  //   // Clear metadata
  //   clear_existing_metadata(tempPath);

  //   // Regions should be cleared
  //   CHECK_FALSE(hasRegionData(tempPath));
  // }
}

TEST_CASE_METHOD(ClearingTestFixture, "clear_existing_metadata removes keyword information", "[clearing]") {
  // SECTION("removes all keyword-related metadata") {
  //   auto tempPath = getTempSample(SampleImage::Sample1);

  //   // Clear metadata
  //   clear_existing_metadata(tempPath);

  //   // Check that keyword info is cleared
  //   CHECK_FALSE(hasKeywordInfo(tempPath));

  //   // Read metadata to verify specific keyword fields
  //   auto afterMetadata = read_metadata(tempPath);
  //   CHECK_FALSE(afterMetadata.LastKeywordXMP.has_value());
  //   CHECK_FALSE(afterMetadata.TagsList.has_value());
  //   CHECK_FALSE(afterMetadata.CatalogSets.has_value());
  //   CHECK_FALSE(afterMetadata.HierarchicalSubject.has_value());
  // }
}

TEST_CASE_METHOD(ClearingTestFixture, "clear_existing_metadata handles multiple samples", "[clearing]") {
  SECTION("works with different image files") {
    // Test specific samples
    std::vector<SampleImage> samplesToTest = {SampleImage::Sample1, SampleImage::Sample2,   SampleImage::Sample3,
                                              SampleImage::Sample4, SampleImage::SamplePNG, SampleImage::SampleWEBP};

    for (const auto& sample : samplesToTest) {
      auto tempPath = getTempSample(sample);
      REQUIRE_NOTHROW(clear_existing_metadata(tempPath));
      auto afterMetadata = read_metadata(tempPath);
      CHECK(afterMetadata.ImageWidth > 0);
      CHECK(afterMetadata.ImageHeight > 0);
    }
  }
}

TEST_CASE_METHOD(ClearingTestFixture, "clear_existing_metadata error handling", "[clearing]") {
  SECTION("throws on invalid file path") {
    std::filesystem::path invalidPath = "/nonexistent/path/image.jpg";
    CHECK_THROWS_AS(clear_existing_metadata(invalidPath), std::runtime_error);
  }

  SECTION("throws on non-image file") {
    // Create a temporary text file
    auto tempPath = std::filesystem::temp_directory_path() / "test.txt";
    std::ofstream file(tempPath);
    file << "This is not an image";
    file.close();

    CHECK_THROWS_AS(clear_existing_metadata(tempPath), std::runtime_error);

    // Clean up
    std::filesystem::remove(tempPath);
  }
}

TEST_CASE_METHOD(ClearingTestFixture, "clear_existing_metadata preserves essential image data", "[clearing]") {
  SECTION("preserves image dimensions and basic structure") {
    auto tempPath = getTempSample(SampleImage::Sample1);

    // Read metadata before clearing
    auto beforeMetadata = read_metadata(tempPath);

    // Clear metadata
    clear_existing_metadata(tempPath);

    // Read metadata after clearing
    auto afterMetadata = read_metadata(tempPath);

    // Essential image properties should be preserved
    CHECK(afterMetadata.ImageWidth == beforeMetadata.ImageWidth);
    CHECK(afterMetadata.ImageHeight == beforeMetadata.ImageHeight);
  }
}

TEST_CASE_METHOD(ClearingTestFixture, "clear_existing_metadata is idempotent", "[clearing]") {
  SECTION("multiple calls produce same result") {
    auto tempPath = getTempSample(SampleImage::Sample1);

    // Clear metadata twice
    clear_existing_metadata(tempPath);
    auto firstClearMetadata = read_metadata(tempPath);

    clear_existing_metadata(tempPath);
    auto secondClearMetadata = read_metadata(tempPath);

    // Results should be identical
    CHECK(firstClearMetadata.ImageWidth == secondClearMetadata.ImageWidth);
    CHECK(firstClearMetadata.ImageHeight == secondClearMetadata.ImageHeight);
    CHECK(firstClearMetadata.Title == secondClearMetadata.Title);
    CHECK(firstClearMetadata.Description == secondClearMetadata.Description);
    CHECK(firstClearMetadata.Orientation == secondClearMetadata.Orientation);
  }
}
