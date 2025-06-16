#pragma once

#include <concepts>
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

  // Equality Comparable
  friend bool operator==(const DimensionsStruct& lhs, const DimensionsStruct& rhs) {
    constexpr double epsilon = 1e-9;
    return std::abs(lhs.H - rhs.H) < epsilon && std::abs(lhs.W - rhs.W) < epsilon && lhs.Unit == rhs.Unit;
  }
};

static_assert(std::copy_constructible<DimensionsStruct>);
static_assert(std::equality_comparable<DimensionsStruct>);
static_assert(XmpSerializable<DimensionsStruct>);
static_assert(PythonBindableRepr<DimensionsStruct>);
static_assert(PythonBindableHashable<DimensionsStruct>);
