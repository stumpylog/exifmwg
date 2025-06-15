#pragma once

#include <optional>
#include <string>

#include <exiv2/exiv2.hpp>

#include "XmpSerializable.hpp"

class XmpAreaStruct {
public:
  double H;
  double W;
  double X;
  double Y;
  std::string Unit;
  std::optional<double> D;

  XmpAreaStruct(double h, double w, double x, double y, const std::string& unit,
                std::optional<double> d = std::nullopt);

  static XmpAreaStruct fromXmp(const Exiv2::XmpData& xmpData, const std::string& baseKey = "");
  void toXmp(Exiv2::XmpData& xmpData, const std::string& basePath = "") const;
  std::string __repr__() const;
};

bool operator==(const XmpAreaStruct& lhs, const XmpAreaStruct& rhs);

static_assert(XmpSerializable<XmpAreaStruct>);
