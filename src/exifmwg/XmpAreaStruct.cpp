#include "XmpAreaStruct.hpp"
#include "Logging.hpp"
#include "XmpUtils.hpp"

XmpAreaStruct::XmpAreaStruct(double h, double w, double x, double y, const std::string& unit, std::optional<double> d) :
    H(h), W(w), X(x), Y(y), Unit(unit), D(d) {
}

XmpAreaStruct XmpAreaStruct::fromXmp(const Exiv2::XmpData& xmpData, const std::string& baseKey) {
  double h = 0.0, w = 0.0, x = 0.0, y = 0.0;
  std::optional<double> d;
  std::string unit = "normalized";

  auto hKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:h"));
  if (hKey != xmpData.end()) {
    h = std::stod(hKey->toString());
  } else {
    throw std::runtime_error("No height found in xmp area struct");
  }
  auto wKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:w"));
  if (wKey != xmpData.end()) {
    w = std::stod(wKey->toString());
  } else {
    throw std::runtime_error("No width found in xmp area struct");
  }
  auto xKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:x"));
  if (xKey != xmpData.end()) {
    x = std::stod(xKey->toString());
  } else {
    throw std::runtime_error("No x found in xmp area struct");
  }
  auto yKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:y"));
  if (yKey != xmpData.end()) {
    y = std::stod(yKey->toString());
  } else {
    throw std::runtime_error("No y found in xmp area struct");
  }
  auto dKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:d"));
  if (dKey != xmpData.end()) {
    d = std::stod(dKey->toString());
  }
  auto unitKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:unit"));
  if (unitKey != xmpData.end()) {
    unit = unitKey->toString();
  }

  return XmpAreaStruct(h, w, x, y, unit, d);
}

void XmpAreaStruct::toXmp(Exiv2::XmpData& xmpData, const std::string& basePath) const {
  LOG_DEBUG("Writing XmpArea to " + basePath);
  xmpData[basePath + "/stArea:h"] = XmpUtils::doubleToStringWithPrecision(H);
  xmpData[basePath + "/stArea:w"] = XmpUtils::doubleToStringWithPrecision(W);
  xmpData[basePath + "/stArea:x"] = XmpUtils::doubleToStringWithPrecision(X);
  xmpData[basePath + "/stArea:y"] = XmpUtils::doubleToStringWithPrecision(Y);
  xmpData[basePath + "/stArea:unit"] = Unit;
  if (D) {
    xmpData[basePath + "/stArea:d"] = std::to_string(*D);
  }
}

std::string XmpAreaStruct::to_string() const {
  std::string repr = "XmpAreaStruct(H=" + std::to_string(H) + ", W=" + std::to_string(W) + ", X=" + std::to_string(X) +
                     ", Y=" + std::to_string(Y) + ", Unit='" + Unit + "'";

  if (D.has_value()) {
    repr += ", D=" + std::to_string(D.value());
  }

  repr += ")";
  return repr;
}

bool operator==(const XmpAreaStruct& lhs, const XmpAreaStruct& rhs) {
  return (lhs.H == rhs.H) && (lhs.W == rhs.W) && (lhs.X == rhs.X) && (lhs.Y == rhs.Y) && (lhs.Unit == rhs.Unit) &&
         (lhs.D == rhs.D);
}

bool operator!=(const XmpAreaStruct& lhs, const XmpAreaStruct& rhs) {
  return !(lhs == rhs);
}
