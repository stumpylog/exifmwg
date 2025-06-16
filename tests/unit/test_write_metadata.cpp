#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "ImageMetadata.hpp"
#include "KeywordInfoModel.hpp"
#include "RegionInfoStruct.hpp"
#include "TestUtils.hpp"

TEST_CASE_METHOD(ImageTestFixture, "write_metadata comprehensive tests", "[writing]") {

  SECTION("Basic metadata writing") {
    auto tempPath = getTempSample(SampleImage::Sample1);

    ImageMetadata metadata(1920, 1080); // Required constructor params
    metadata.Title = "Test Title";
    metadata.Description = "Test Description";
    metadata.Orientation = 6;

    REQUIRE_NOTHROW(metadata.toFile(tempPath));

    // Verify written data
    ImageMetadata readBack(tempPath);
    CHECK(readBack.Title == "Test Title");
    CHECK(readBack.Description == "Test Description");
    CHECK(readBack.Orientation == 6);
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

  SECTION("Keyword arrays writing") {
    auto tempPath = getTempSample(SampleImage::Sample3);

    ImageMetadata metadata(1920, 1080);
    metadata.LastKeywordXMP = std::vector<std::string>{"keyword1", "keyword2", "keyword3"};
    metadata.TagsList = std::vector<std::string>{"tag1", "tag2"};
    metadata.CatalogSets = std::vector<std::string>{"set1", "set2", "set3"};
    metadata.HierarchicalSubject = std::vector<std::string>{"subject1", "subject2"};

    REQUIRE_NOTHROW(metadata.toFile(tempPath));

    ImageMetadata readBack(tempPath);
    CHECK(readBack.LastKeywordXMP == metadata.LastKeywordXMP);
    CHECK(readBack.TagsList == metadata.TagsList);
    CHECK(readBack.CatalogSets == metadata.CatalogSets);
    CHECK(readBack.HierarchicalSubject == metadata.HierarchicalSubject);
  }

  SECTION("Empty keyword arrays") {
    auto tempPath = getTempSample(SampleImage::Sample1);

    ImageMetadata metadata(1920, 1080);
    metadata.LastKeywordXMP = std::vector<std::string>{};
    metadata.TagsList = std::vector<std::string>{};

    REQUIRE_NOTHROW(metadata.toFile(tempPath));

    ImageMetadata readBack(tempPath);
    CHECK_FALSE(readBack.LastKeywordXMP.has_value());
    CHECK_FALSE(readBack.TagsList.has_value());
  }

  SECTION("Keywords with special characters") {
    // TODO: Investigate failure
    // auto tempPath = getTempSample(SampleImage::Sample2));

    // ImageMetadata metadata(1920, 1080);
    // metadata.LastKeywordXMP =
    //     std::vector<std::string>{"keyword,with,comma", "keyword;with;semicolon", "keyword with spaces"};

    // REQUIRE_NOTHROW(metadata.toFile());

    // ImageMetadata readBack(tempPath);
    // CHECK(readBack.LastKeywordXMP == metadata.LastKeywordXMP);
  }

  SECTION("RegionInfo and KeywordInfo writing") {
    auto tempPath = getTempSample(SampleImage::Sample4);

    // TODO: Need to define a constructor I think?

    // ImageMetadata metadata(1920, 1080);

    // KeywordInfoModel model({"Animal/Mammal/Dog"}, '/');
    // metadata.RegionInfo = RegionInfoStruct::RegionStruct({0.4, 0.3, 0.1, 0.2, "normalized"}, "Person", "Face",
    // "Smiling"); metadata.KeywordInfo = model;

    // REQUIRE_NOTHROW(metadata.toFile());

    // ImageMetadata readBack(tempPath);
    // CHECK(readBack.RegionInfo.has_value());
    // CHECK(readBack.KeywordInfo.has_value());
  }

  SECTION("Partial metadata updates") {
    const auto tempPath = getTempSample(SampleImage::Sample1);

    // Write initial metadata
    ImageMetadata initial(1920, 1080);
    initial.Title = "Original Title";
    initial.Country = "Original Country";
    std::cerr << "Writing initial" << std::endl;
    initial.toFile(tempPath);

    // Update only some fields
    ImageMetadata update(1920, 1080);
    update.Title = "Updated Title";
    // Country not set - should remain unchanged

    std::cerr << "Writing updated" << std::endl;
    REQUIRE_NOTHROW(update.toFile(tempPath));

    std::cerr << "Reading updated" << std::endl;
    ImageMetadata readBack(tempPath);
    CHECK(readBack.Title == "Updated Title");
    CHECK(readBack.Country == "Original Country");
  }

  SECTION("Overwriting existing metadata") {
    auto tempPath = getTempSample(SampleImage::Sample2);

    // Write initial keywords
    ImageMetadata initial(1920, 1080);
    initial.LastKeywordXMP = std::vector<std::string>{"old1", "old2"};
    initial.toFile(tempPath);

    // Overwrite with new keywords
    ImageMetadata update(1920, 1080);
    update.LastKeywordXMP = std::vector<std::string>{"new1", "new2", "new3"};

    REQUIRE_NOTHROW(update.toFile(tempPath));

    ImageMetadata readBack(tempPath);
    CHECK(readBack.LastKeywordXMP->size() == 3);
    CHECK((*readBack.LastKeywordXMP)[0] == "new1");
    CHECK((*readBack.LastKeywordXMP)[1] == "new2");
    CHECK((*readBack.LastKeywordXMP)[2] == "new3");
  }

  SECTION("Unicode and international characters") {
    auto tempPath = getTempSample(SampleImage::Sample3);

    ImageMetadata metadata(1920, 1080);
    metadata.Title = "测试标题";
    metadata.Description = "Tëst Dëscriptïön";
    metadata.Country = "日本";
    metadata.LastKeywordXMP = std::vector<std::string>{"키워드", "كلمة", "слово"};

    REQUIRE_NOTHROW(metadata.toFile(tempPath));

    ImageMetadata readBack(tempPath);
    CHECK(readBack.Title == "测试标题");
    CHECK(readBack.Description == "Tëst Dëscriptïön");
    CHECK(readBack.Country == "日本");
    CHECK(readBack.LastKeywordXMP == metadata.LastKeywordXMP);
  }

  SECTION("Error cases") {
    SECTION("Non-existent file") {
      std::filesystem::path nonExistent = "/non/existent/file.jpg";

      REQUIRE_FALSE(std::filesystem::exists(nonExistent));

      ImageMetadata metadata(1920, 1080);
      metadata.Title = "Test";

      CHECK_THROWS_AS(metadata.toFile(nonExistent), std::runtime_error);
    }

    SECTION("Invalid file format") {
      // Create a temp text file with .jpg extension
      auto tempPath = std::filesystem::temp_directory_path() / "invalid.jpg";
      std::ofstream file(tempPath);
      file << "This is not an image file";
      file.close();

      ImageMetadata metadata(1920, 1080);
      metadata.Title = "Test";

      CHECK_THROWS_AS(metadata.toFile(), std::runtime_error);

      std::filesystem::remove(tempPath);
    }

    SECTION("Read-only file") {
      auto tempPath = getTempSample(SampleImage::Sample1);

      // Make file read-only
      std::filesystem::permissions(tempPath, std::filesystem::perms::owner_read | std::filesystem::perms::group_read |
                                                 std::filesystem::perms::others_read);

      ImageMetadata metadata(1920, 1080);
      metadata.Title = "Test";

      CHECK_THROWS_AS(metadata.toFile(), std::runtime_error);

      // Restore permissions for cleanup
      std::filesystem::permissions(tempPath, std::filesystem::perms::owner_all | std::filesystem::perms::group_read |
                                                 std::filesystem::perms::others_read);
    }
  }

  SECTION("Large metadata sets") {
    auto tempPath = getTempSample(SampleImage::Sample4);

    ImageMetadata metadata(1920, 1080);
    metadata.Title = std::string(1000, 'A'); // Very long title

    // Large keyword array
    std::vector<std::string> largeKeywords;
    for (int i = 0; i < 100; ++i) {
      largeKeywords.push_back("keyword" + std::to_string(i));
    }
    metadata.LastKeywordXMP = largeKeywords;

    REQUIRE_NOTHROW(metadata.toFile(tempPath));

    ImageMetadata readBack(tempPath);
    CHECK(readBack.Title == metadata.Title);
    CHECK(readBack.LastKeywordXMP->size() == 100);
  }

  SECTION("All optional fields unset") {
    auto tempPath = getTempSample(SampleImage::Sample1);

    ImageMetadata emptyMetadata(1920, 1080); // All optionals are nullopt/empty

    REQUIRE_NOTHROW(emptyMetadata.toFile(tempPath));

    // Should not crash, file should still be readable
    REQUIRE_NOTHROW(ImageMetadata(tempPath));
  }
}
