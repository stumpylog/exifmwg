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
    KeywordStruct(const std::string& keyword, const std::vector<KeywordStruct>& children = {},
                  std::optional<bool> applied = std::nullopt);

    // XMP serialization
    static KeywordStruct fromXmp(const Exiv2::XmpData& xmpData, const std::string& basePath);
    void toXmp(Exiv2::XmpData& xmpData, const std::string& basePath) const;

    // Python bindable
    std::string to_string() const;

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

private:
  std::vector<KeywordStruct> mergeKeywordVectors(const std::vector<KeywordStruct>& vec1,
                                                 const std::vector<KeywordStruct>& vec2);
  KeywordStruct* findOrCreateChild(std::vector<KeywordStruct>& children, const std::string& keyword);
  std::optional<bool> mergeApplied(const std::optional<bool>& a, const std::optional<bool>& b);
};

bool operator==(const KeywordInfoModel::KeywordStruct& lhs, const KeywordInfoModel::KeywordStruct& rhs);
bool operator!=(const KeywordInfoModel::KeywordStruct& lhs, const KeywordInfoModel::KeywordStruct& rhs);

bool operator==(const KeywordInfoModel& lhs, const KeywordInfoModel& rhs);
bool operator!=(const KeywordInfoModel& lhs, const KeywordInfoModel& rhs);

static_assert(XmpSerializableWithKey<KeywordInfoModel::KeywordStruct>);
static_assert(PythonBindableBase<KeywordInfoModel::KeywordStruct>);

static_assert(XmpSerializable<KeywordInfoModel>);
static_assert(PythonBindableBase<KeywordInfoModel>);
