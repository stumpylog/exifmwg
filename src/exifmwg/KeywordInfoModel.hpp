#pragma once
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
    KeywordStruct(std::string keyword, const std::vector<KeywordStruct>& children = {},
                  std::optional<bool> applied = std::nullopt);

    // XMP serialization
    static KeywordStruct fromXmp(const Exiv2::XmpData& xmpData, const std::string& basePath);
    void toXmp(Exiv2::XmpData& xmpData, const std::string& basePath) const;

    // Python bindable
    std::string to_string() const;

    friend bool operator==(const KeywordStruct& lhs, const KeywordStruct& rhs) {
      return (lhs.Keyword == rhs.Keyword) && (lhs.Applied == rhs.Applied) && (lhs.Children == rhs.Children);
    }

  private:
    void writeChildrenToXmp(Exiv2::XmpData& xmpData, const std::string& basePath) const;
  };

  std::vector<KeywordStruct> Hierarchy;

  // Constructors
  KeywordInfoModel(const std::vector<KeywordStruct>& hierarchy);
  KeywordInfoModel(const std::vector<std::string>& delimitedStrings, char delimiter = '/');

  // XMP serialization
  static KeywordInfoModel fromXmp(const Exiv2::XmpData& xmpData);
  void toXmp(Exiv2::XmpData& xmpData) const;

  // Python bindable
  std::string to_string() const;

  // Operators
  KeywordInfoModel& operator|=(const KeywordInfoModel& other);
  KeywordInfoModel operator|(const KeywordInfoModel& other) const;

  friend bool operator==(const KeywordInfoModel& lhs, const KeywordInfoModel& rhs) {
    return lhs.Hierarchy == rhs.Hierarchy;
  }

private:
  std::vector<KeywordStruct> mergeKeywordVectors(const std::vector<KeywordStruct>& vec1,
                                                 const std::vector<KeywordStruct>& vec2);
  static KeywordStruct* findOrCreateChild(std::vector<KeywordStruct>& children, const std::string& keyword);
  static std::optional<bool> mergeApplied(const std::optional<bool>& a, const std::optional<bool>& b);
};

static_assert(std::copy_constructible<KeywordInfoModel::KeywordStruct>);
static_assert(std::equality_comparable<KeywordInfoModel::KeywordStruct>);
static_assert(XmpSerializableWithKey<KeywordInfoModel::KeywordStruct>);
static_assert(PythonBindableRepr<KeywordInfoModel::KeywordStruct>);

static_assert(std::copy_constructible<KeywordInfoModel>);
static_assert(std::equality_comparable<KeywordInfoModel>);
static_assert(XmpSerializable<KeywordInfoModel>);
static_assert(PythonBindableRepr<KeywordInfoModel>);
