#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

// Add Exiv2 header for direct XMP data manipulation
#include <exiv2/exiv2.hpp>

#include "Errors.hpp"
#include "ImageMetadata.hpp"
#include "KeywordInfoModel.hpp"
#include "RegionInfoStruct.hpp"
#include "TestUtils.hpp"

// Helper to erase an XMP key if it exists
void clearXmpKey(Exiv2::XmpData& xmpData, const std::string& key) {
  auto it = xmpData.findKey(Exiv2::XmpKey(key));
  if (it != xmpData.end()) {
    xmpData.erase(it);
  }
}

TEST_CASE_METHOD(ImageTestFixture, "write_metadata comprehensive tests", "[writing]") {

  SECTION("Basic metadata writing") {
    auto tempPath = getTempSample(SampleImage::Sample1);

    ImageMetadata metadata(1920, 1080);
    metadata.Title = "Test Title";
    metadata.Description = "Test Description";
    metadata.Orientation = ExifOrientation::Rotate270CW;

    REQUIRE_NOTHROW(metadata.toFile(tempPath));

    ImageMetadata readBack(tempPath);
    CHECK(readBack.Title == "Test Title");
    CHECK(readBack.Description == "Test Description");
    CHECK(readBack.Orientation == ExifOrientation::Rotate270CW);
  }

  SECTION("Location metadata writing") {
    auto tempPath = getTempSample(SampleImage::Sample2);

    ImageMetadata metadata(1920, 1080);
    metadata.Country = "United States";
    metadata.State = "California";
    metadata.City = "San Francisco";
    metadata.Location = "Golden Gate Bridge";

    REQUIRE_NOTHROW(metadata.toFile(tempPath));

    ImageMetadata readBack(tempPath);
    CHECK(readBack.Country == "United States");
    CHECK(readBack.State == "California");
    CHECK(readBack.City == "San Francisco");
    CHECK(readBack.Location == "Golden Gate Bridge");
  }

  SECTION("Hierarchical keywords and compatibility tag writing") {
    auto tempPath = getTempSample(SampleImage::Sample4);

    ImageMetadata metadata(1920, 1080);
    KeywordInfoModel model(std::vector<std::string>{"Place/USA/Washington", "Event/Holiday", "People/Family"}, '/');
    metadata.KeywordInfo = model;

    REQUIRE_NOTHROW(metadata.toFile(tempPath));

    // Verify model read-back
    ImageMetadata readBack(tempPath);
    REQUIRE(readBack.KeywordInfo.has_value());
    CHECK(*readBack.KeywordInfo == model);

    // Verify raw XMP compatibility tags are written correctly
    auto image = Exiv2::ImageFactory::open(tempPath.string());
    REQUIRE(image.get() != nullptr);
    image->readMetadata();
    const Exiv2::XmpData& xmpData = image->xmpData();

    // Check Lightroom tag (pipe-delimited, sorted)
    auto lrIt = xmpData.findKey(Exiv2::XmpKey("Xmp.lr.hierarchicalSubject"));
    REQUIRE(lrIt != xmpData.end());
    // Keywords are sorted: Event, People, Place
    CHECK(lrIt->toString() == "Event|Holiday,People|Family,Place|USA|Washington");

    // Check DigiKam tag (slash-delimited, sorted)
    auto dkIt = xmpData.findKey(Exiv2::XmpKey("Xmp.digiKam.TagsList"));
    REQUIRE(dkIt != xmpData.end());
    CHECK(dkIt->toString() == "Event/Holiday,People/Family,Place/USA/Washington");

    // Check Microsoft tag (should be identical to DigiKam)
    auto msIt = xmpData.findKey(Exiv2::XmpKey("Xmp.MicrosoftPhoto.LastKeywordXMP"));
    REQUIRE(msIt != xmpData.end());
    CHECK(msIt->toString() == dkIt->toString());
  }

  SECTION("Keyword reading and merging from legacy formats") {
    auto tempPath = getTempSample(SampleImage::Sample2);

    // 1. Manually prepare an image with various legacy keyword tags
    {
      auto image = Exiv2::ImageFactory::open(tempPath.string());
      REQUIRE(image.get() != nullptr);
      image->readMetadata();
      Exiv2::XmpData& xmpData = image->xmpData();

      // Clear any existing keyword data to ensure a clean slate
      clearXmpKey(xmpData, "Xmp.mwg-kw.Keywords");
      clearXmpKey(xmpData, "Xmp.lr.hierarchicalSubject");
      clearXmpKey(xmpData, "Xmp.digiKam.TagsList");
      clearXmpKey(xmpData, "Xmp.MicrosoftPhoto.LastKeywordXMP");
      clearXmpKey(xmpData, "Xmp.mediapro.CatalogSets");

      // Add data to different legacy fields
      xmpData["Xmp.lr.hierarchicalSubject"] = "Source|Lightroom,Place|USA";
      xmpData["Xmp.digiKam.TagsList"] = "Source/DigiKam,Event/Birthday";
      xmpData["Xmp.MicrosoftPhoto.LastKeywordXMP"] = "People/John Doe";

      image->setXmpData(xmpData);
      image->writeMetadata();
    }

    // 2. Read metadata using the library, which should merge the tags
    ImageMetadata readBack(tempPath);

    // 3. Verify the merged and sorted hierarchy is correct
    REQUIRE(readBack.KeywordInfo.has_value());

    // This is the expected structure after merging and sorting all sources
    KeywordInfoModel expectedModel(std::vector<std::string>{"Event/Birthday", "People/John Doe", "Place/USA",
                                                            "Source/DigiKam", "Source/Lightroom"},
                                   '/');

    CHECK(*readBack.KeywordInfo == expectedModel);
  }

  SECTION("Overwriting existing keywords") {
    auto tempPath = getTempSample(SampleImage::Sample2);

    // Write initial keywords
    ImageMetadata initial(1920, 1080);
    initial.KeywordInfo = KeywordInfoModel(std::vector<std::string>{"old/keyword1", "old/keyword2"});
    initial.toFile(tempPath);

    // Overwrite with new keywords
    ImageMetadata update(1920, 1080);
    update.KeywordInfo = KeywordInfoModel(std::vector<std::string>{"new/keyword1", "new/keyword2", "new/keyword3"});

    REQUIRE_NOTHROW(update.toFile(tempPath));

    ImageMetadata readBack(tempPath);
    REQUIRE(readBack.KeywordInfo.has_value());
    CHECK(*readBack.KeywordInfo == *update.KeywordInfo);

    // Also verify raw tags to ensure old ones were cleared
    auto image = Exiv2::ImageFactory::open(tempPath.string());
    REQUIRE(image.get() != nullptr);
    image->readMetadata();
    const Exiv2::XmpData& xmpData = image->xmpData();
    auto lrIt = xmpData.findKey(Exiv2::XmpKey("Xmp.lr.hierarchicalSubject"));
    REQUIRE(lrIt != xmpData.end());
    CHECK(lrIt->toString() == "new|keyword1,new|keyword2,new|keyword3");
  }

  SECTION("Unicode and international characters") {
    auto tempPath = getTempSample(SampleImage::Sample3);

    ImageMetadata metadata(1920, 1080);
    metadata.Title = "测试标题";
    metadata.Country = "日本";
    metadata.KeywordInfo = KeywordInfoModel(std::vector<std::string>{"키워드/하나", "كلمة", "слово/мир"});

    REQUIRE_NOTHROW(metadata.toFile(tempPath));

    ImageMetadata readBack(tempPath);
    CHECK(readBack.Title == "测试标题");
    CHECK(readBack.Country == "日本");
    REQUIRE(readBack.KeywordInfo.has_value());
    CHECK(*readBack.KeywordInfo == *metadata.KeywordInfo);
  }
}

TEST_CASE_METHOD(ImageTestFixture, "ImageMetadata file operations", "[ImageMetadata]") {

  SECTION("save() - updates original file") {
    auto tempPath = getTempSample(SampleImage::Sample1);
    ImageMetadata metadata(tempPath);
    metadata.Title = "Updated Title";
    metadata.Description = "Updated Description";
    metadata.Orientation = ExifOrientation::Rotate180;

    REQUIRE_NOTHROW(metadata.save());

    // Read back and verify changes were written
    ImageMetadata readBack(tempPath);
    CHECK(readBack.Title == "Updated Title");
    CHECK(readBack.Description == "Updated Description");
    CHECK(readBack.Orientation == ExifOrientation::Rotate180);
  }

  SECTION("save() - throws when no original file path") {
    ImageMetadata metadata(1920, 1080);
    metadata.Title = "Test Title";

    CHECK_THROWS_AS(metadata.save(), FileAccessError);
    CHECK_THROWS_WITH(metadata.save(), Catch::Matchers::ContainsSubstring("No original file path available"));
  }

  SECTION("toFile() - updates existing file") {
    auto tempPath = getTempSample(SampleImage::Sample1);
    auto anotherTempPath = getTempSample(SampleImage::Sample1);

    ImageMetadata metadata(tempPath);
    metadata.Title = "Test Title";
    metadata.Description = "Test Description";
    metadata.Orientation = ExifOrientation::Rotate270CW;

    REQUIRE_NOTHROW(metadata.toFile(anotherTempPath));

    // Verify changes were written to the target file
    ImageMetadata readBack(anotherTempPath);
    CHECK(readBack.Title == "Test Title");
    CHECK(readBack.Description == "Test Description");
    CHECK(readBack.Orientation == ExifOrientation::Rotate270CW);

    // Verify original file wasn't modified
    ImageMetadata original(tempPath);
    CHECK(original.Title != "Test Title"); // Assuming original doesn't have this title
  }

  SECTION("toFile() - throws when target file doesn't exist") {
    auto tempPath = getTempSample(SampleImage::Sample1);
    ImageMetadata metadata(tempPath);
    metadata.Title = "Test Title";

    std::filesystem::path nonExistentPath = "/tmp/does_not_exist.jpg";

    CHECK_THROWS_AS(metadata.toFile(nonExistentPath), FileAccessError);
    CHECK_THROWS_WITH(metadata.toFile(nonExistentPath),
                      Catch::Matchers::ContainsSubstring("Target file does not exist"));
  }

  SECTION("toFile() - works with metadata-only object") {
    auto tempPath = getTempSample(SampleImage::Sample1);

    // Create metadata object without original file
    ImageMetadata metadata(1920, 1080);
    metadata.Title = "Metadata Only Title";
    metadata.Description = "Metadata Only Description";
    metadata.Orientation = ExifOrientation::Horizontal;

    REQUIRE_NOTHROW(metadata.toFile(tempPath));

    ImageMetadata readBack(tempPath);
    CHECK(readBack.Title == "Metadata Only Title");
    CHECK(readBack.Description == "Metadata Only Description");
    CHECK(readBack.Orientation == ExifOrientation::Horizontal);
  }

  SECTION("copyTo() - creates new file with updated metadata") {
    auto tempPath = getTempSample(SampleImage::Sample1);
    std::filesystem::path newPath = std::filesystem::temp_directory_path() / "copied_image.jpg";

    // Clean up any existing file
    if (std::filesystem::exists(newPath)) {
      std::filesystem::remove(newPath);
    }

    ImageMetadata metadata(tempPath);
    metadata.Title = "Copied Title";
    metadata.Description = "Copied Description";
    metadata.Orientation = ExifOrientation::Rotate90CW;

    REQUIRE_NOTHROW(metadata.copyTo(newPath));
    REQUIRE(std::filesystem::exists(newPath));

    // Verify new file has the updated metadata
    ImageMetadata readBack(newPath);
    CHECK(readBack.Title == "Copied Title");
    CHECK(readBack.Description == "Copied Description");
    CHECK(readBack.Orientation == ExifOrientation::Rotate90CW);

    // Verify original file wasn't modified
    ImageMetadata original(tempPath);
    CHECK(original.Title != "Copied Title"); // Assuming original doesn't have this title

    // Clean up
    std::filesystem::remove(newPath);
  }

  SECTION("copyTo() - creates parent directories") {
    auto tempPath = getTempSample(SampleImage::Sample1);
    std::filesystem::path newDir = std::filesystem::temp_directory_path() / "test_dir" / "subdir";
    std::filesystem::path newPath = newDir / "copied_image.jpg";

    // Ensure directory doesn't exist
    if (std::filesystem::exists(newDir)) {
      std::filesystem::remove_all(newDir.parent_path());
    }

    ImageMetadata metadata(tempPath);
    metadata.Title = "Directory Test";

    REQUIRE_NOTHROW(metadata.copyTo(newPath));
    REQUIRE(std::filesystem::exists(newPath));
    REQUIRE(std::filesystem::exists(newDir));

    ImageMetadata readBack(newPath);
    CHECK(readBack.Title == "Directory Test");

    // Clean up
    std::filesystem::remove_all(newDir.parent_path());
  }

  SECTION("copyTo() - throws when no original file available") {
    ImageMetadata metadata(1920, 1080);
    metadata.Title = "No Original";

    std::filesystem::path newPath = std::filesystem::temp_directory_path() / "should_not_exist.jpg";

    CHECK_THROWS_AS(metadata.copyTo(newPath), FileAccessError);
    CHECK_THROWS_WITH(metadata.copyTo(newPath),
                      Catch::Matchers::ContainsSubstring("No original file available to copy from"));

    CHECK_FALSE(std::filesystem::exists(newPath));
  }

  SECTION("copyTo() - overwrites existing file") {
    auto tempPath = getTempSample(SampleImage::Sample1);
    auto existingPath = getTempSample(SampleImage::Sample1);

    ImageMetadata metadata(tempPath);
    metadata.Title = "Overwrite Test";
    metadata.Description = "This should overwrite";

    REQUIRE_NOTHROW(metadata.copyTo(existingPath));

    ImageMetadata readBack(existingPath);
    CHECK(readBack.Title == "Overwrite Test");
    CHECK(readBack.Description == "This should overwrite");
  }

  SECTION("Round-trip test: save -> toFile -> copyTo") {
    auto originalPath = getTempSample(SampleImage::Sample1);
    auto existingPath = getTempSample(SampleImage::Sample1);
    std::filesystem::path copyPath = std::filesystem::temp_directory_path() / "roundtrip_copy.jpg";

    // Clean up any existing copy
    if (std::filesystem::exists(copyPath)) {
      std::filesystem::remove(copyPath);
    }

    // Start with original file
    ImageMetadata metadata(originalPath);
    metadata.Title = "Round Trip Title";
    metadata.Description = "Round Trip Description";
    metadata.Orientation = ExifOrientation::MirrorVertical;

    // Save to original
    REQUIRE_NOTHROW(metadata.save());

    // Write to existing file
    REQUIRE_NOTHROW(metadata.toFile(existingPath));

    // Copy to new location
    REQUIRE_NOTHROW(metadata.copyTo(copyPath));

    // Verify all three files have the same metadata
    ImageMetadata original(originalPath);
    ImageMetadata existing(existingPath);
    ImageMetadata copied(copyPath);

    CHECK(original.Title == "Round Trip Title");
    CHECK(existing.Title == "Round Trip Title");
    CHECK(copied.Title == "Round Trip Title");

    CHECK(original.Description == "Round Trip Description");
    CHECK(existing.Description == "Round Trip Description");
    CHECK(copied.Description == "Round Trip Description");

    CHECK(original.Orientation == ExifOrientation::MirrorVertical);
    CHECK(existing.Orientation == ExifOrientation::MirrorVertical);
    CHECK(copied.Orientation == ExifOrientation::MirrorVertical);

    // Clean up
    std::filesystem::remove(copyPath);
  }
}
