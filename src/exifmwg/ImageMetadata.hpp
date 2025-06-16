#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "DimensionsStruct.hpp"
#include "KeywordInfoModel.hpp"
#include "PythonBindable.hpp"
#include "RegionInfoStruct.hpp"
#include "XmpAreaStruct.hpp"

class ImageMetadata {
public:
  int ImageHeight;
  int ImageWidth;
  std::optional<std::string> Title;
  std::optional<std::string> Description;
  std::optional<RegionInfoStruct> RegionInfo;
  std::optional<int> Orientation;
  std::optional<std::vector<std::string>> LastKeywordXMP;
  std::optional<std::vector<std::string>> TagsList;
  std::optional<std::vector<std::string>> CatalogSets;
  std::optional<std::vector<std::string>> HierarchicalSubject;
  std::optional<KeywordInfoModel> KeywordInfo;
  std::optional<std::string> Country;
  std::optional<std::string> City;
  std::optional<std::string> State;
  std::optional<std::string> Location;

  ImageMetadata() = default;

  ImageMetadata(const std::filesystem::path& path);

  // This is used mostly in Python level testing, to construct expected structures
  ImageMetadata(int imageHeight, int imageWidth, std::optional<std::string> title = std::nullopt,
                std::optional<std::string> description = std::nullopt,
                std::optional<RegionInfoStruct> regionInfo = std::nullopt,
                std::optional<int> orientation = std::nullopt,
                std::optional<std::vector<std::string>> lastKeywordXMP = std::nullopt,
                std::optional<std::vector<std::string>> tagsList = std::nullopt,
                std::optional<std::vector<std::string>> catalogSets = std::nullopt,
                std::optional<std::vector<std::string>> hierarchicalSubject = std::nullopt,
                std::optional<KeywordInfoModel> keywordInfo = std::nullopt,
                std::optional<std::string> country = std::nullopt, std::optional<std::string> city = std::nullopt,
                std::optional<std::string> state = std::nullopt, std::optional<std::string> location = std::nullopt);

  void toFile(const std::optional<std::filesystem::path>& newPath = std::nullopt);
  static void clearFile(const std::filesystem::path& path);

  // Python bindable
  std::string to_string() const;

  friend bool operator==(const ImageMetadata& lhs, const ImageMetadata& rhs) {
    return (lhs.ImageHeight == rhs.ImageHeight) && (lhs.ImageWidth == rhs.ImageWidth) && (lhs.Title == rhs.Title) &&
           (lhs.Description == rhs.Description) && (lhs.RegionInfo == rhs.RegionInfo) &&
           (lhs.Orientation == rhs.Orientation) && (lhs.LastKeywordXMP == rhs.LastKeywordXMP) &&
           (lhs.TagsList == rhs.TagsList) && (lhs.CatalogSets == rhs.CatalogSets) &&
           (lhs.HierarchicalSubject == rhs.HierarchicalSubject) && (lhs.KeywordInfo == rhs.KeywordInfo) &&
           (lhs.Country == rhs.Country) && (lhs.City == rhs.City) && (lhs.State == rhs.State) &&
           (lhs.Location == rhs.Location);
  }

private:
  std::optional<std::filesystem::path> m_originalPath;
};

// Equality operators
static_assert(std::copy_constructible<ImageMetadata>);
static_assert(std::equality_comparable<ImageMetadata>);
static_assert(PythonBindableRepr<ImageMetadata>);
