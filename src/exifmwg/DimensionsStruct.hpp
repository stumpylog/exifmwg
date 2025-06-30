#pragma once

#include <concepts>
#include <string>
#include <tuple>

#include <exiv2/exiv2.hpp>

#include "PythonBindable.hpp"
#include "XmpSerializable.hpp"

class DimensionsStruct {
public:
  double H;
  double W;
  std::string Unit;

  DimensionsStruct(double h, double w, std::string unit);

  // XMP serialization
  static DimensionsStruct fromXmp(const Exiv2::XmpData& xmpData, const std::string& baseKey = "");
  void toXmp(Exiv2::XmpData& xmpData, const std::string& basePath = "") const;

  // Python bindable
  std::string to_string() const;

  // Equality Comparable
  friend bool operator==(const DimensionsStruct& lhs, const DimensionsStruct& rhs) {
    constexpr double epsilon = 1e-9;
    return std::abs(lhs.H - rhs.H) < epsilon && std::abs(lhs.W - rhs.W) < epsilon && lhs.Unit == rhs.Unit;
  }
};

// TODO: Consider making this a tuple and hashing that
namespace std {
template <> struct hash<DimensionsStruct> {
  std::size_t operator()(const DimensionsStruct& p) const noexcept {
    std::size_t h1 = std::hash<double>{}(p.H);
    std::size_t h2 = std::hash<double>{}(p.W);
    std::size_t h3 = std::hash<std::string>{}(p.Unit);
    return h1 ^ (h2 << 1) ^ (h3 << 2);
  }
};
} // namespace std

static_assert(std::copy_constructible<DimensionsStruct>);
static_assert(std::equality_comparable<DimensionsStruct>);
static_assert(XmpSerializable<DimensionsStruct>);
static_assert(PythonBindableRepr<DimensionsStruct>);
