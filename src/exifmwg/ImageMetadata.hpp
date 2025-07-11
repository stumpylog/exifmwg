#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "DimensionsStruct.hpp"
#include "KeywordInfoModel.hpp"
#include "Orientation.hpp"
#include "PythonBindable.hpp"
#include "RegionInfoStruct.hpp"
#include "XmpAreaStruct.hpp"

class ImageMetadata {
public:
  uint32_t ImageHeight;
  uint32_t ImageWidth;
  std::optional<std::string> Title;
  std::optional<std::string> Description;
  std::optional<RegionInfoStruct> RegionInfo;
  std::optional<ExifOrientation> Orientation;
  std::optional<KeywordInfoModel> KeywordInfo;
  std::optional<std::string> Country;
  std::optional<std::string> City;
  std::optional<std::string> State;
  std::optional<std::string> Location;

  ImageMetadata() = default;

  explicit ImageMetadata(const std::filesystem::path& path);

  // This is used mostly in Python level testing, to construct expected structures
  ImageMetadata(int imageHeight, int imageWidth, std::optional<std::string> title = std::nullopt,
                std::optional<std::string> description = std::nullopt,
                std::optional<RegionInfoStruct> regionInfo = std::nullopt,
                std::optional<ExifOrientation> orientation = std::nullopt,
                std::optional<KeywordInfoModel> keywordInfo = std::nullopt,
                std::optional<std::string> country = std::nullopt, std::optional<std::string> city = std::nullopt,
                std::optional<std::string> state = std::nullopt, std::optional<std::string> location = std::nullopt);

  void toFile(const std::optional<std::filesystem::path>& newPath = std::nullopt);
  void clearFile(const std::optional<std::filesystem::path>& path = std::nullopt);

  // Python bindable
  std::string to_string() const;

  friend bool operator==(const ImageMetadata& lhs, const ImageMetadata& rhs) {
    return (lhs.ImageHeight == rhs.ImageHeight) && (lhs.ImageWidth == rhs.ImageWidth) && (lhs.Title == rhs.Title) &&
           (lhs.Description == rhs.Description) && (lhs.RegionInfo == rhs.RegionInfo) &&
           (lhs.Orientation == rhs.Orientation) && (lhs.KeywordInfo == rhs.KeywordInfo) &&
           (lhs.Country == rhs.Country) && (lhs.City == rhs.City) && (lhs.State == rhs.State) &&
           (lhs.Location == rhs.Location);
  }

private:
  std::optional<std::filesystem::path> m_originalPath;

  // Private helper methods for reading metadata
  void readOrientation(const Exiv2::ExifData& exifData);
  void readTitleAndDescription(const Exiv2::XmpData& xmpData, const Exiv2::IptcData& iptcData);
  void readLocationData(const Exiv2::XmpData& xmpData, const Exiv2::IptcData& iptcData);
  void readRegionInfo(const Exiv2::XmpData& xmpData);
  void readKeywordInfo(const Exiv2::XmpData& xmpData);

  // Private helper methods for writing metadata
  void writeTitleAndDescription(Exiv2::XmpData& xmpData, Exiv2::IptcData& iptcData);
  void writeOrientation(Exiv2::ExifData& exifData);
  void writeLocationData(Exiv2::XmpData& xmpData, Exiv2::IptcData& iptcData);
  void writeRegionInfo(Exiv2::XmpData& xmpData);
  void writeKeywordInfo(Exiv2::XmpData& xmpData);

  // Private helper methods for clearing metadata
  void clearRegionInfo(Exiv2::XmpData& xmpData);
  void clearOrientation(Exiv2::ExifData& exifData);
  void clearKeywordInfo(Exiv2::XmpData& xmpData);
  void clearTitleAndDescription(Exiv2::XmpData& xmpData, Exiv2::IptcData& iptcData, Exiv2::ExifData& exifData);
};

// Equality operators
static_assert(std::copy_constructible<ImageMetadata>);
static_assert(std::equality_comparable<ImageMetadata>);
static_assert(PythonBindableRepr<ImageMetadata>);
