#include "RegionInfoStruct.hpp"
#include "Logging.hpp"
#include "XmpUtils.hpp"

RegionInfoStruct::RegionStruct::RegionStruct(const XmpAreaStruct& area, const std::string& name,
                                             const std::string& type, std::optional<std::string> description) :
    Area(area), Name(name), Type(type), Description(std::move(description)) {
}

RegionInfoStruct::RegionStruct RegionInfoStruct::RegionStruct::fromXmp(const Exiv2::XmpData& xmpData,
                                                                       const std::string& baseKey) {
  XmpAreaStruct area = XmpAreaStruct::fromXmp(xmpData, baseKey + "/mwg-rs:Area");

  std::string name_val = "";
  std::string type_val = "";
  std::optional<std::string> desc_val;

  auto nameKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/mwg-rs:Name"));
  if (nameKey != xmpData.end()) {
    name_val = XmpUtils::cleanXmpText(nameKey->toString());
  } else {
    throw std::runtime_error("No name found in region info struct");
  }

  auto typeKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/mwg-rs:Type"));
  if (typeKey != xmpData.end()) {
    type_val = typeKey->toString();
  } else {
    throw std::runtime_error("No type found in region info struct");
  }

  auto descKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/mwg-rs:Description"));
  if (descKey != xmpData.end()) {
    desc_val = descKey->toString();
  }

  return RegionInfoStruct::RegionStruct(area, name_val, type_val, desc_val);
}

std::string RegionInfoStruct::RegionStruct::to_string() const {
  std::string repr = "RegionStruct(Area=" + Area.to_string() + ", Name='" + Name + "', Type='" + Type + "'";

  if (Description.has_value()) {
    repr += ", Description='" + Description.value() + "'";
  }

  repr += ")";
  return repr;
}

void RegionInfoStruct::RegionStruct::toXmp(Exiv2::XmpData& xmpData, const std::string& itemPath) const {
  // Write the Area struct
  std::string areaPath = itemPath + "/mwg-rs:Area";
  Area.toXmp(xmpData, areaPath);
  InternalLogger::debug("Writing Region to " + itemPath);

  // Write other region properties
  xmpData[itemPath + "/mwg-rs:Name"] = Name;
  xmpData[itemPath + "/mwg-rs:Type"] = Type;

  if (Description) {
    xmpData[itemPath + "/mwg-rs:Description"] = *Description;
  }
}

RegionInfoStruct::RegionInfoStruct(const DimensionsStruct& appliedToDimensions,
                                   const std::vector<RegionInfoStruct::RegionStruct>& regionList) :
    AppliedToDimensions(appliedToDimensions), RegionList(regionList) {
}

RegionInfoStruct RegionInfoStruct::fromXmp(const Exiv2::XmpData& xmpData) {
  // Parse AppliedToDimensions
  DimensionsStruct appliedToDimensions_val =
      DimensionsStruct::fromXmp(xmpData, "Xmp.mwg-rs.Regions/mwg-rs:AppliedToDimensions");
  std::vector<RegionInfoStruct::RegionStruct> regionList_val;

  // Parse RegionList
  int regionIndex = 1;
  while (true) {
    std::string baseKey = "Xmp.mwg-rs.Regions/mwg-rs:RegionList[" + std::to_string(regionIndex) + "]";
    InternalLogger::debug("Checking key " + baseKey);

    // Check if any keys exist that start with baseKey
    bool regionExists = false;
    for (const auto& item : xmpData) {
      if (item.key().find(baseKey) == 0) {
        regionExists = true;
        break;
      }
    }

    if (!regionExists) {
      break;
    }

    InternalLogger::debug("Reading key " + baseKey);
    regionList_val.push_back(RegionInfoStruct::RegionStruct::fromXmp(xmpData, baseKey));
    regionIndex++;
  }

  return RegionInfoStruct(appliedToDimensions_val, regionList_val);
}

void RegionInfoStruct::toXmp(Exiv2::XmpData& xmpData) const {
  InternalLogger::debug("Writing MWG Regions hierarchy");

  // Clear existing MWG Regions data
  XmpUtils::clearXmpKey(xmpData, "Xmp.mwg-rs.Regions");

  // Write AppliedToDimensions first
  AppliedToDimensions.toXmp(xmpData, "Xmp.mwg-rs.Regions/mwg-rs:AppliedToDimensions");

  const std::string baseRegionList = "Xmp.mwg-rs.Regions/mwg-rs:RegionList";
  xmpData[baseRegionList] = "";

  // Write regions if any exist
  for (size_t i = 0; i < RegionList.size(); ++i) {
    const auto& region = RegionList[i];
    const std::string itemPath = baseRegionList + "[" + std::to_string(i + 1) + "]";

    region.toXmp(xmpData, itemPath);
  }

  InternalLogger::debug("Wrote " + std::to_string(RegionList.size()) + " regions.");
}

std::string RegionInfoStruct::to_string() const {
  std::string repr = "RegionInfoStruct(AppliedToDimensions=" + AppliedToDimensions.to_string() + ", RegionList=[";

  for (size_t i = 0; i < RegionList.size(); ++i) {
    if (i > 0) {
      repr += ", ";
    }
    repr += RegionList[i].to_string();
  }

  repr += "])";
  return repr;
}
