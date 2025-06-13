#include <filesystem>
#include <fstream>
#include <stdexcept>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "Image.hpp"
#include "TestUtils.hpp"

TEST_CASE_METHOD(ImageTestFixture, "Image::save comprehensive tests", "[image][writing]") {

  SECTION("Basic metadata writing") {
    auto tempPath = getTempSample(SampleImage::Sample1);
    {
      Image image(tempPath);
      image.Title = "Test Title";
      image.Description = "Test Description";
      image.Orientation = 6;
      REQUIRE_NOTHROW(image.save());
    }

    Image readBack(tempPath);
    CHECK(readBack.Title == "Test Title");
    CHECK(readBack.Description == "Test Description");
    CHECK(readBack.Orientation == 6);
  }

  SECTION("Location metadata writing") {
    auto tempPath = getTempSample(SampleImage::Sample2);
    {
      Image image(tempPath);
      image.Country = "United States";
      image.State = "California";
      image.City = "San Francisco";
      image.Location = "Golden Gate Bridge";
      REQUIRE_NOTHROW(image.save());
    }

    Image readBack(tempPath);
    CHECK(readBack.Country == "United States");
    CHECK(readBack.State == "California");
    CHECK(readBack.City == "San Francisco");
    CHECK(readBack.Location == "Golden Gate Bridge");
  }

  SECTION("Keyword arrays writing") {
    auto tempPath = getTempSample(SampleImage::Sample3);
    auto keywords = std::vector<std::string>{"keyword1", "keyword2", "keyword3"};
    {
      Image image(tempPath);
      image.LastKeywordXMP = keywords;
      image.TagsList = keywords;
      REQUIRE_NOTHROW(image.save());
    }

    Image readBack(tempPath);
    REQUIRE(readBack.LastKeywordXMP.has_value());
    CHECK(readBack.LastKeywordXMP.value() == keywords);
    REQUIRE(readBack.TagsList.has_value());
    CHECK(readBack.TagsList.value() == keywords);
  }

  SECTION("Partial metadata updates") {
    auto tempPath = getTempSample(SampleImage::Sample1);
    {
      Image image(tempPath);
      image.Title = "Original Title";
      image.Country = "Original Country";
      image.save();
    }
    {
      Image imageToUpdate(tempPath);
      imageToUpdate.Title = "Updated Title";
      imageToUpdate.save();
    }

    Image readBack(tempPath);
    CHECK(readBack.Title == "Updated Title");
    CHECK(readBack.Country == "Original Country");
  }

  SECTION("Saving to a new path") {
    auto originalPath = getTempSample(SampleImage::Sample1);
    auto newPath = originalPath.parent_path() / "new_saved_image.jpg";
    {
      Image image(originalPath);
      image.Title = "Saved to a new location";
      image.save(newPath);

      CHECK(image.getPath() == newPath);
    }

    REQUIRE(std::filesystem::exists(newPath));
    Image readBack(newPath);
    CHECK(readBack.Title == "Saved to a new location");

    Image originalReadBack(originalPath);
    CHECK_FALSE(originalReadBack.Title.has_value());
  }

  SECTION("Error handling for save()") {
    auto tempPath = getTempSample(SampleImage::Sample1);
    Image image(tempPath);
    image.Title = "Test";

    SECTION("Read-only file") {
      std::filesystem::permissions(tempPath, std::filesystem::perms::owner_read | std::filesystem::perms::group_read |
                                                 std::filesystem::perms::others_read);
      CHECK_THROWS_AS(image.save(), std::runtime_error);
      std::filesystem::permissions(tempPath, std::filesystem::perms::owner_all);
    }
  }
}
