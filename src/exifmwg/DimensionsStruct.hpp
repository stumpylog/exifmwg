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

  static DimensionsStruct fromXmp(const Exiv2::XmpData& xmpData, const std::string& baseKey = "");
  void toXmp(Exiv2::XmpData& xmpData, const std::string& basePath = "") const;
  std::string to_string() const;
};

bool operator==(const DimensionsStruct& lhs, const DimensionsStruct& rhs);
bool operator!=(const DimensionsStruct& lhs, const DimensionsStruct& rhs);

static_assert(XmpSerializable<DimensionsStruct>);
static_assert(PythonBindable<DimensionsStruct>);
