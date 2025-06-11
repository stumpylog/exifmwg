#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "DimensionsStruct.hpp"
#include "KeywordInfoModel.hpp"
#include "RegionInfoStruct.hpp"
#include "XmpAreaStruct.hpp"

namespace fs = std::filesystem;

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
};

// Equality operators
bool operator==(const ImageMetadata& lhs, const ImageMetadata& rhs);
bool operator!=(const ImageMetadata& lhs, const ImageMetadata& rhs);
