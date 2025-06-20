#include "DimensionsStruct.hpp"
#include "XmpUtils.hpp"

/**
 * @brief Constructs a DimensionsStruct object.
 *
 * Initializes the dimension structure with height, width, and unit of measurement.
 *
 * @param h The height value.
 * @param w The width value.
 * @param unit The unit of measurement (e.g., "pixel", "inch").
 */
DimensionsStruct::DimensionsStruct(double h, double w, const std::string& unit) : H(h), W(w), Unit(unit) {
}

/**
 * @brief Parses a DimensionsStruct from XMP data.
 *
 * Reads height, width, and unit fields from the provided XMP data, using the given base key.
 * Throws a std::runtime_error if any required field is missing.
 *
 * @param xmpData The Exiv2::XmpData object containing the XMP metadata.
 * @param baseKey The base key prefix for the dimension fields.
 * @return A populated DimensionsStruct object.
 * @throws std::runtime_error if any field is missing or cannot be converted.
 */
DimensionsStruct DimensionsStruct::fromXmp(const Exiv2::XmpData& xmpData, const std::string& baseKey) {

  double h = 0.0, w = 0.0;
  std::string unit;

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

/**
 * @brief Serializes the DimensionsStruct to XMP data.
 *
 * Writes the height, width, and unit as XMP fields under the given base path.
 *
 * @param xmpData The Exiv2::XmpData object to modify.
 * @param basePath The base key prefix for writing dimension fields.
 */
void DimensionsStruct::toXmp(Exiv2::XmpData& xmpData, const std::string& basePath) const {
  xmpData[basePath + "/stDim:h"] = XmpUtils::doubleToStringWithPrecision(this->H);
  xmpData[basePath + "/stDim:w"] = XmpUtils::doubleToStringWithPrecision(this->W);
  xmpData[basePath + "/stDim:unit"] = this->Unit;
}

/**
 * @brief Compares two DimensionsStruct objects for equality.
 *
 * Uses an epsilon threshold for floating-point height and width comparison, and exact match for unit.
 *
 * @param lhs The left-hand side DimensionsStruct.
 * @param rhs The right-hand side DimensionsStruct.
 * @return true if all fields match within tolerance, false otherwise.
 */
bool operator==(const DimensionsStruct& lhs, const DimensionsStruct& rhs) {

  constexpr double epsilon = 1e-9;

  return (std::abs(lhs.H - rhs.H) < epsilon) && (std::abs(lhs.W - rhs.W) < epsilon) && (lhs.Unit == rhs.Unit);
}
