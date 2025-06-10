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

struct ImageMetadata {
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

  ImageMetadata(int imageHeight, int imageWidth,
                std::optional<std::string> title,
                std::optional<std::string> description,
                std::optional<RegionInfoStruct> regionInfo,
                std::optional<int> orientation,
                std::optional<std::vector<std::string>> lastKeywordXMP,
                std::optional<std::vector<std::string>> tagsList,
                std::optional<std::vector<std::string>> catalogSets,
                std::optional<std::vector<std::string>> hierarchicalSubject,
                std::optional<KeywordInfoModel> keywordInfo,
                std::optional<std::string> country,
                std::optional<std::string> city,
                std::optional<std::string> state,
                std::optional<std::string> location);
};

// Equality operators
bool operator==(const ImageMetadata &lhs, const ImageMetadata &rhs);
