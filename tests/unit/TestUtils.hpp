#pragma once
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>
#include <chrono>
#include <filesystem>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
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
    } else if constexpr (std::is_same_v<std::decay_t<decltype(opt)>, std::optional<ExifOrientation>>) {
      oss << (opt ? orientation_to_string(*opt) : "None");

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

template <> struct StringMaker<std::vector<std::string>> {
  static std::string convert(const std::vector<std::string>& vec) {
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
};

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
        << ", Location=" << formatOptional(metadata.Location) << "}";
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

enum class SampleImage { Sample1, Sample2, Sample3, Sample4, SamplePNG, SampleWEBP };

class ImageTestFixture {
protected:
  mutable std::vector<std::filesystem::path> tempPaths_;
  static const std::unordered_map<SampleImage, std::string> sampleFiles_;

  std::filesystem::path getSampleImagePath(SampleImage sampleNumber) const {
    std::filesystem::path sourceFile(__FILE__);
    std::filesystem::path imagesDir = sourceFile.parent_path().parent_path() / "samples" / "images";
    return imagesDir / sampleFiles_.at(sampleNumber);
  }

  std::filesystem::path createTempCopy(const std::filesystem::path& originalPath) const {
    std::filesystem::path temp = std::filesystem::temp_directory_path() /
                                 ("test_" + originalPath.filename().string() + "_" +
                                  std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));

    std::filesystem::copy_file(originalPath, temp, std::filesystem::copy_options::overwrite_existing);
    return temp;
  }

public:
  ~ImageTestFixture() {
    // Clean up any temp files that were created
    for (const auto& tempPath : tempPaths_) {
      if (std::filesystem::exists(tempPath)) {
        std::filesystem::remove(tempPath);
      }
    }
  }

  // Get original path (read-only)
  std::filesystem::path getOriginalSample(SampleImage sampleNumber) const {
    std::filesystem::path path = getSampleImagePath(sampleNumber);
    if (!std::filesystem::exists(path)) {
      throw std::runtime_error("Sample image not found");
    }
    return path;
  }

  // Get temp copy (creates on first access)
  std::filesystem::path getTempSample(SampleImage sampleNumber) const {
    std::filesystem::path originalPath = getOriginalSample(sampleNumber);
    std::filesystem::path tempPath = createTempCopy(originalPath);
    this->tempPaths_.push_back(tempPath);
    return tempPath;
  }

  // Check if a sample exists without creating a copy
  bool hasSample(SampleImage sampleNumber) const {
    return std::filesystem::exists(getSampleImagePath(sampleNumber));
  }
};
