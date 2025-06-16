#pragma once

#include <concepts>
#include <optional>
#include <string>

#include <exiv2/exiv2.hpp>

#include "PythonBindable.hpp"
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

  // XMP serialization
  static XmpAreaStruct fromXmp(const Exiv2::XmpData& xmpData, const std::string& baseKey = "");
  void toXmp(Exiv2::XmpData& xmpData, const std::string& basePath = "") const;

  // Python bindable
  std::string to_string() const;

  // Equality Comparable
  friend bool operator==(const XmpAreaStruct& lhs, const XmpAreaStruct& rhs) {
    constexpr double epsilon = 1e-9;
    return (std::abs(lhs.H - rhs.H) < epsilon) && (std::abs(lhs.W - rhs.W) < epsilon) &&
           (std::abs(lhs.X - rhs.X) < epsilon) && (std::abs(lhs.Y - rhs.Y) < epsilon) && (lhs.Unit == rhs.Unit) &&
           // It's equal if:
           // 1. Both have values AND their values are approximately equal, OR
           // 2. Neither has a value (both are nullopt).
           ((lhs.D.has_value() && rhs.D.has_value() && std::abs(lhs.D.value() - rhs.D.value()) < epsilon) ||
            (!lhs.D.has_value() && !rhs.D.has_value()));
  }
};

static_assert(std::copy_constructible<XmpAreaStruct>);
static_assert(std::equality_comparable<XmpAreaStruct>);
static_assert(XmpSerializable<XmpAreaStruct>);
static_assert(PythonBindableRepr<XmpAreaStruct>);
