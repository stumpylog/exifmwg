#pragma once

#include <optional>
#include <string>

#include <exiv2/exiv2.hpp>

#include "DimensionsStruct.hpp"
#include "PythonBindable.hpp"
#include "XmpAreaStruct.hpp"
#include "XmpSerializable.hpp"

class RegionInfoStruct {
public:
  class RegionStruct {
  public:
    XmpAreaStruct Area;
    std::string Name;
    std::string Type;
    std::optional<std::string> Description;

    RegionStruct(const XmpAreaStruct& area, const std::string& name, const std::string& type,
                 std::optional<std::string> description);

    // XMP serialization
    static RegionStruct fromXmp(const Exiv2::XmpData& xmpData, const std::string& baseKey);
    void toXmp(Exiv2::XmpData& xmpData, const std::string& itemPath) const;

    // Python bindable
    std::string to_string() const;

    friend bool operator==(const RegionStruct& lhs, const RegionStruct& rhs) {
      return (lhs.Area == rhs.Area) && (lhs.Name == rhs.Name) && (lhs.Type == rhs.Type) &&
             (lhs.Description == rhs.Description);
    }
  };

  DimensionsStruct AppliedToDimensions;
  std::vector<RegionStruct> RegionList;

  RegionInfoStruct(const DimensionsStruct& appliedToDimensions, const std::vector<RegionStruct>& regionList);

  // XMP serialization
  static RegionInfoStruct fromXmp(const Exiv2::XmpData& xmpData);
  void toXmp(Exiv2::XmpData& xmpData) const;

  // Python bindable
  std::string to_string() const;

  friend bool operator==(const RegionInfoStruct& lhs, const RegionInfoStruct& rhs) {
    return lhs.AppliedToDimensions == rhs.AppliedToDimensions && lhs.RegionList == rhs.RegionList;
  }
};

static_assert(std::copy_constructible<RegionInfoStruct::RegionStruct>);
static_assert(std::equality_comparable<RegionInfoStruct::RegionStruct>);
static_assert(XmpSerializableWithKey<RegionInfoStruct::RegionStruct>);

static_assert(std::copy_constructible<RegionInfoStruct>);
static_assert(std::equality_comparable<RegionInfoStruct>);
static_assert(XmpSerializable<RegionInfoStruct>);
