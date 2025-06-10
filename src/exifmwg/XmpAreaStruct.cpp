#include "XmpAreaStruct.hpp"
#include "XmpUtils.hpp"

XmpAreaStruct::XmpAreaStruct(double h, double w, double x, double y,
                             const std::string &unit, std::optional<double> d)
    : H(h), W(w), X(x), Y(y), Unit(unit), D(d) {}

XmpAreaStruct XmpAreaStruct::fromXmp(const Exiv2::XmpData &xmpData,
                                     const std::string &baseKey) {
  double h = 0.0, w = 0.0, x = 0.0, y = 0.0;
  std::optional<double> d;
  std::string unit = "normalized";

  auto hKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:h"));
  if (hKey != xmpData.end()) {
    h = std::stod(hKey->toString());
  }
  auto wKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:w"));
  if (wKey != xmpData.end()) {
    w = std::stod(wKey->toString());
  }
  auto xKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:x"));
  if (xKey != xmpData.end()) {
    x = std::stod(xKey->toString());
  }
  auto yKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:y"));
  if (yKey != xmpData.end()) {
    y = std::stod(yKey->toString());
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

void XmpAreaStruct::toXmp(Exiv2::XmpData &xmpData,
                          const std::string &basePath) const {
  xmpData[basePath + "/stArea:h"] = XmpUtils::doubleToStringWithPrecision(H);
  xmpData[basePath + "/stArea:w"] = XmpUtils::doubleToStringWithPrecision(W);
  xmpData[basePath + "/stArea:x"] = XmpUtils::doubleToStringWithPrecision(X);
  xmpData[basePath + "/stArea:y"] = XmpUtils::doubleToStringWithPrecision(Y);
  xmpData[basePath + "/stArea:unit"] = Unit;
  if (D) {
    xmpData[basePath + "/stArea:d"] = std::to_string(*D);
  }
}

bool operator==(const XmpAreaStruct &lhs, const XmpAreaStruct &rhs) {
  return (lhs.H == rhs.H) && (lhs.W == rhs.W) && (lhs.X == rhs.X) &&
         (lhs.Y == rhs.Y) && (lhs.Unit == rhs.Unit) && (lhs.D == rhs.D);
}
