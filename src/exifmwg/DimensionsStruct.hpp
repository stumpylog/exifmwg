#pragma once

#include <string>

#include <exiv2/exiv2.hpp>

#include "PythonBindable.hpp"
#include "XmpSerializable.hpp"

class DimensionsStruct {
public:
  double H;
  double W;
  std::string Unit;

  DimensionsStruct(double h, double w, const std::string& unit);

  // XMP serialization
  static DimensionsStruct fromXmp(const Exiv2::XmpData& xmpData, const std::string& baseKey = "");
  void toXmp(Exiv2::XmpData& xmpData, const std::string& basePath = "") const;

  // Python bindable
  std::string to_string() const;
  std::size_t hash() const;
};

bool operator==(const DimensionsStruct& lhs, const DimensionsStruct& rhs);
bool operator!=(const DimensionsStruct& lhs, const DimensionsStruct& rhs);

static_assert(XmpSerializable<DimensionsStruct>);
static_assert(PythonBindableBase<DimensionsStruct>);
static_assert(PythonBindableHashable<DimensionsStruct>);
