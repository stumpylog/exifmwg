#include "DimensionsStruct.hpp"
#include "XmpUtils.hpp"

DimensionsStruct::DimensionsStruct(double h, double w, const std::string& unit) : H(h), W(w), Unit(unit) {
}

DimensionsStruct DimensionsStruct::fromXmp(const Exiv2::XmpData& xmpData, const std::string& baseKey) {

  double h = 0.0, w = 0.0;
  std::string unit = "pixel";

  // Parse individual dimension fields
  auto hKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stDim:h"));
  if (hKey != xmpData.end()) {
    h = std::stod(hKey->toString());
  } else {
    throw std::runtime_error("No height found in dimensions struct");
  }

  auto wKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stDim:w"));
  if (wKey != xmpData.end()) {
    w = std::stod(wKey->toString());
  } else {
    throw std::runtime_error("No width found in dimensions struct");
  }

  auto unitKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stDim:unit"));
  if (unitKey != xmpData.end()) {
    unit = unitKey->toString();
  } else {
    throw std::runtime_error("No unit found in dimensions struct");
  }

  return DimensionsStruct(h, w, unit);
}

void DimensionsStruct::toXmp(Exiv2::XmpData& xmpData, const std::string& basePath) const {
  xmpData[basePath + "/stDim:h"] = XmpUtils::doubleToStringWithPrecision(this->H);
  xmpData[basePath + "/stDim:w"] = XmpUtils::doubleToStringWithPrecision(this->W);
  xmpData[basePath + "/stDim:unit"] = this->Unit;
}

bool operator==(const DimensionsStruct& lhs, const DimensionsStruct& rhs) {
  return (lhs.H == rhs.H) && (lhs.W == rhs.W) && (lhs.Unit == rhs.Unit);
}
