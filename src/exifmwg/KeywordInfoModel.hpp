#pragma once
#include <compare>
#include <optional>
#include <string>
#include <vector>

#include <exiv2/exiv2.hpp>

#include "PythonBindable.hpp"
#include "XmpSerializable.hpp"

class KeywordInfoModel {
public:
  class KeywordStruct {
  public:
    std::string Keyword;
    std::optional<bool> Applied;
    std::vector<KeywordStruct> Children;

    // Constructors
    explicit KeywordStruct(std::string keyword, const std::vector<KeywordStruct>& children = {},
                           std::optional<bool> applied = std::nullopt);

    // XMP serialization
    static KeywordStruct fromXmp(const Exiv2::XmpData& xmpData, const std::string& basePath);
    void toXmp(Exiv2::XmpData& xmpData, const std::string& basePath) const;

    // Python bindable
    std::string to_string() const;

    friend bool operator==(const KeywordStruct& lhs, const KeywordStruct& rhs) {
      return (lhs.Keyword == rhs.Keyword) && (lhs.Applied == rhs.Applied) && (lhs.Children == rhs.Children);
    }

    auto operator<=>(const KeywordStruct& other) const {
      if (auto cmp = Keyword <=> other.Keyword; cmp != 0) {
        return cmp;
      }

      if (Applied.has_value() && other.Applied.has_value()) {
        if (auto cmp = *Applied <=> *other.Applied; cmp != 0) {
          return cmp;
        }
      } else if (Applied.has_value() && !other.Applied.has_value()) {
        return std::strong_ordering::greater; // A value is "greater" than no value
      } else if (!Applied.has_value() && other.Applied.has_value()) {
        return std::strong_ordering::less; // No value is "less" than a value
      }

      return std::strong_ordering::equal;
    }

  private:
    void writeChildrenToXmp(Exiv2::XmpData& xmpData, const std::string& basePath) const;
  };

  std::vector<KeywordStruct> Hierarchy;

  // Constructors
  explicit KeywordInfoModel(const std::vector<KeywordStruct>& hierarchy);
  explicit KeywordInfoModel(const std::vector<std::string>& delimitedStrings, char delimiter = '/');

  // XMP serialization
  static KeywordInfoModel fromXmp(const Exiv2::XmpData& xmpData);
  void toXmp(Exiv2::XmpData& xmpData) const;

  // IPTC serialization (special case)
  // TODO:IPTC 255??

  // Python bindable
  std::string to_string() const;

  // Operators
  KeywordInfoModel& operator|=(const KeywordInfoModel& other);
  KeywordInfoModel operator|(const KeywordInfoModel& other) const;

  friend bool operator==(const KeywordInfoModel& lhs, const KeywordInfoModel& rhs) {
    return lhs.Hierarchy == rhs.Hierarchy;
  }

private:
  static std::vector<KeywordStruct> mergeKeywordVectors(const std::vector<KeywordStruct>& vec1,
                                                        const std::vector<KeywordStruct>& vec2);
  static KeywordStruct* findOrCreateChild(std::vector<KeywordStruct>& children, const std::string& keyword);
  static std::optional<bool> mergeApplied(const std::optional<bool>& a, const std::optional<bool>& b);

  static void sortKeywordVector(std::vector<KeywordStruct>& keywords);

  // Helper parsers
  static KeywordStruct parseHierarchicalPath(const std::string& path, char delimiter, bool leafApplied = true);
  static std::vector<KeywordStruct> parseDelimitedPaths(const std::string& data, char pathDelim, char listDelim = ',');
  static std::vector<KeywordStruct> parseACDSeeXML(const std::string& xmlData);
  static void mergeKeywordIntoHierarchy(std::vector<KeywordStruct>& hierarchy, const KeywordStruct& keyword);
  // Output helpers
  void writeHierarchicalPaths(std::vector<std::string>& paths, const KeywordStruct& keyword,
                              const std::string& currentPath, char delimiter) const;
  std::string buildDelimitedPaths(char delimiter) const;
};

static_assert(std::copy_constructible<KeywordInfoModel::KeywordStruct>);
static_assert(std::totally_ordered<KeywordInfoModel::KeywordStruct>, "KeywordStruct must be totally ordered!");
static_assert(XmpSerializableWithKey<KeywordInfoModel::KeywordStruct>);
static_assert(PythonBindableRepr<KeywordInfoModel::KeywordStruct>);

static_assert(std::copy_constructible<KeywordInfoModel>);
static_assert(std::equality_comparable<KeywordInfoModel>);
static_assert(XmpSerializable<KeywordInfoModel>);
static_assert(PythonBindableRepr<KeywordInfoModel>);
