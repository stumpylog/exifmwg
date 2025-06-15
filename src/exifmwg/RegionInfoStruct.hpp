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
  };

  DimensionsStruct AppliedToDimensions;
  std::vector<RegionStruct> RegionList;

  RegionInfoStruct(const DimensionsStruct& appliedToDimensions, const std::vector<RegionStruct>& regionList);

  // XMP serialization
  static RegionInfoStruct fromXmp(const Exiv2::XmpData& xmpData);
  void toXmp(Exiv2::XmpData& xmpData) const;

  // Python bindable
  std::string to_string() const;
};

bool operator==(const RegionInfoStruct::RegionStruct& lhs, const RegionInfoStruct::RegionStruct& rhs);
bool operator!=(const RegionInfoStruct::RegionStruct& lhs, const RegionInfoStruct::RegionStruct& rhs);
bool operator==(const RegionInfoStruct& lhs, const RegionInfoStruct& rhs);
bool operator!=(const RegionInfoStruct& lhs, const RegionInfoStruct& rhs);

static_assert(XmpSerializable<RegionInfoStruct>);
static_assert(XmpSerializableWithKey<RegionInfoStruct::RegionStruct>);

static_assert(PythonBindableBase<RegionInfoStruct>);
static_assert(PythonBindableBase<RegionInfoStruct::RegionStruct>);
