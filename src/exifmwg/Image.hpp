#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "exiv2/exiv2.hpp"

#include "KeywordInfoModel.hpp"
#include "RegionInfoStruct.hpp"

class Image {
public:
  /**
   * @brief Constructs an Image object by reading metadata from the given path.
   * @param path The path to the image file.
   */
  explicit Image(const std::filesystem::path& path);

  /**
   * @brief Saves metadata changes back to a file.
   * If newPath is provided, the original image is copied to the new location
   * and the metadata is written to the new file. Otherwise, it overwrites
   * the original file.
   * @param newPath An optional path to save the new file to.
   */
  void save(const std::optional<std::filesystem::path>& newPath = std::nullopt);

  /**
   * @brief Clears all supported metadata fields from the object and saves the changes.
   * This is a destructive operation.
   */
  void clearMetadata();

  // --- Public Metadata Fields ---
  int ImageHeight = 0;
  int ImageWidth = 0;
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

  std::filesystem::path getPath() const;

private:
  std::filesystem::path m_originalPath;

  void _readMetadata();
  void _writeMetadata(const std::filesystem::path& targetPath);
  void _clearMetadataFields(const std::filesystem::path& targetPath);
};

// Equality operators
bool operator==(const Image& lhs, const Image& rhs);
bool operator!=(const Image& lhs, const Image& rhs);
