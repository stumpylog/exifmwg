#pragma once
#include <optional>
#include <string>
#include <vector>

#include <exiv2/exiv2.hpp>

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

    std::string __repr__() const;

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

  // Operators
  KeywordInfoModel& operator|=(const KeywordInfoModel& other);
  KeywordInfoModel operator|(const KeywordInfoModel& other) const;

  std::string __repr__() const;

private:
  std::vector<KeywordStruct> mergeKeywordVectors(const std::vector<KeywordStruct>& vec1,
                                                 const std::vector<KeywordStruct>& vec2);
  KeywordStruct* findOrCreateChild(std::vector<KeywordStruct>& children, const std::string& keyword);
  std::optional<bool> mergeApplied(const std::optional<bool>& a, const std::optional<bool>& b);
};

bool operator==(const KeywordInfoModel::KeywordStruct& lhs, const KeywordInfoModel::KeywordStruct& rhs);
bool operator==(const KeywordInfoModel& lhs, const KeywordInfoModel& rhs);

static_assert(XmpSerializable<KeywordInfoModel>);
static_assert(XmpSerializableWithKey<KeywordInfoModel::KeywordStruct>);
