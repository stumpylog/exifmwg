#pragma once
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "DimensionsStruct.hpp"
#include "ImageMetadata.hpp"
#include "KeywordInfoModel.hpp"
#include "RegionInfoStruct.hpp"
#include "XmpAreaStruct.hpp"

namespace Catch {

// Helper function to format optional values
template <typename T> inline std::string formatOptional(const std::optional<T>& opt) {
  if (opt) {
    std::ostringstream oss;
    if constexpr (std::is_same_v<T, std::string>) {
      oss << "\"" << *opt << "\"";
    } else {
      oss << *opt;
    }
    return oss.str();
  }
  return "nullopt";
}

// Helper function to format vector of strings
inline std::string formatStringVector(const std::vector<std::string>& vec) {
  std::ostringstream oss;
  oss << "[";
  for (size_t i = 0; i < vec.size(); ++i) {
    if (i > 0)
      oss << ", ";
    oss << "\"" << vec[i] << "\"";
  }
  oss << "]";
  return oss.str();
}

// Helper function to format optional vector of strings
inline std::string formatOptionalStringVector(const std::optional<std::vector<std::string>>& opt) {
  return opt ? formatStringVector(*opt) : "nullopt";
}

template <> struct StringMaker<XmpAreaStruct> {
  static std::string convert(const XmpAreaStruct& area) {
    std::ostringstream oss;
    oss << "H: " << area.H << ", W: " << area.W << ", X: " << area.X << ", Y: " << area.Y << ", Unit: \"" << area.Unit
        << "\""
        << ", D: " << formatOptional(area.D);
    return oss.str();
  }
};

template <> struct StringMaker<DimensionsStruct> {
  static std::string convert(const DimensionsStruct& dims) {
    std::ostringstream oss;
    oss << "H: " << dims.H << ", W: " << dims.W << ", Unit: \"" << dims.Unit << "\"";
    return oss.str();
  }
};

template <> struct StringMaker<RegionInfoStruct::RegionStruct> {
  static std::string convert(const RegionInfoStruct::RegionStruct& region) {
    std::ostringstream oss;
    oss << "RegionStruct { "
        << "Area: " << StringMaker<XmpAreaStruct>::convert(region.Area) << ", Name: \"" << region.Name << "\""
        << ", Type: \"" << region.Type << "\""
        << ", Description: " << formatOptional(region.Description) << " }";
    return oss.str();
  }
};

template <> struct StringMaker<RegionInfoStruct> {
  static std::string convert(const RegionInfoStruct& info) {
    std::ostringstream oss;
    oss << "RegionInfoStruct {\n  AppliedToDimensions: "
        << StringMaker<DimensionsStruct>::convert(info.AppliedToDimensions) << ",\n  RegionList: [\n";

    for (const auto& region : info.RegionList) {
      oss << "    " << StringMaker<RegionInfoStruct::RegionStruct>::convert(region) << ",\n";
    }

    oss << "  ]\n}";
    return oss.str();
  }
};

template <> struct StringMaker<ImageMetadata> {
  static std::string convert(const ImageMetadata& metadata) {
    std::ostringstream oss;
    oss << "ImageMetadata{"
        << "ImageHeight=" << metadata.ImageHeight << ", ImageWidth=" << metadata.ImageWidth
        << ", Title=" << formatOptional(metadata.Title) << ", Description=" << formatOptional(metadata.Description)
        << ", Orientation=" << formatOptional(metadata.Orientation) << ", Country=" << formatOptional(metadata.Country)
        << ", City=" << formatOptional(metadata.City) << ", State=" << formatOptional(metadata.State)
        << ", Location=" << formatOptional(metadata.Location)
        << ", LastKeywordXMP=" << formatOptionalStringVector(metadata.LastKeywordXMP)
        << ", TagsList=" << formatOptionalStringVector(metadata.TagsList)
        << ", CatalogSets=" << formatOptionalStringVector(metadata.CatalogSets)
        << ", HierarchicalSubject=" << formatOptionalStringVector(metadata.HierarchicalSubject) << "}";
    return oss.str();
  }
};

// Generic optional StringMakers
template <> struct StringMaker<std::optional<std::string>> {
  static std::string convert(const std::optional<std::string>& opt) {
    return formatOptional(opt);
  }
};

template <> struct StringMaker<std::optional<int>> {
  static std::string convert(const std::optional<int>& opt) {
    return formatOptional(opt);
  }
};

template <> struct StringMaker<std::optional<std::vector<std::string>>> {
  static std::string convert(const std::optional<std::vector<std::string>>& opt) {
    return formatOptionalStringVector(opt);
  }
};

} // namespace Catch
