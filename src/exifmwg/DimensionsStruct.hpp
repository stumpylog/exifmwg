#pragma once

#include <string>

#include <exiv2/exiv2.hpp>

#include "XmpSerializable.hpp"

class DimensionsStruct {
public:
  double H;
  double W;
  std::string Unit;

  DimensionsStruct(double h, double w, const std::string &unit);

  static DimensionsStruct fromXmp(const Exiv2::XmpData &xmpData,
                                  const std::string &baseKey = "");
  void toXmp(Exiv2::XmpData &xmpData, const std::string &basePath = "") const;
};

bool operator==(const DimensionsStruct &lhs, const DimensionsStruct &rhs);

static_assert(XmpSerializable<DimensionsStruct>);
