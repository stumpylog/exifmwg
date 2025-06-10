#include "RegionInfoStruct.hpp"
#include "utils.hpp"

RegionInfoStruct::RegionStruct::RegionStruct(
    const XmpAreaStruct &area, const std::string &name, const std::string &type,
    std::optional<std::string> description)
    : Area(area), Name(name), Type(type), Description(std::move(description)) {}

RegionInfoStruct::RegionStruct
RegionInfoStruct::RegionStruct::fromXmp(const Exiv2::XmpData &xmpData,
                                        const std::string &baseKey) {
  XmpAreaStruct area =
      XmpAreaStruct::fromXmp(xmpData, baseKey + "/mwg-rs:Area");

  std::string name_val = "";
  std::string type_val = "";
  std::optional<std::string> desc_val;

  auto nameKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/mwg-rs:Name"));
  if (nameKey != xmpData.end()) {
    name_val = clean_xmp_text(nameKey->toString());
  }

  auto typeKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/mwg-rs:Type"));
  if (typeKey != xmpData.end()) {
    type_val = typeKey->toString();
  }

  auto descKey =
      xmpData.findKey(Exiv2::XmpKey(baseKey + "/mwg-rs:Description"));
  if (descKey != xmpData.end()) {
    desc_val = descKey->toString();
  }

  return RegionInfoStruct::RegionStruct(area, name_val, type_val, desc_val);
}

void RegionInfoStruct::RegionStruct::toXmp(Exiv2::XmpData &xmpData,
                                           const std::string &itemPath) const {
  // Write the Area struct
  std::string areaPath = itemPath + "/mwg-rs:Area";
  Area.toXmp(xmpData, areaPath);

  // Write other region properties
  xmpData[itemPath + "/mwg-rs:Name"] = Name;
  xmpData[itemPath + "/mwg-rs:Type"] = Type;

  if (Description) {
    xmpData[itemPath + "/mwg-rs:Description"] = *Description;
  }
}

RegionInfoStruct::RegionInfoStruct(const DimensionsStruct &appliedToDimensions,
                                   const std::vector<Region> &regionList)
    : AppliedToDimensions(appliedToDimensions), RegionList(regionList) {}

RegionInfoStruct RegionInfoStruct::fromXmp(const Exiv2::XmpData &xmpData) {
  // Parse AppliedToDimensions
  DimensionsStruct appliedToDimensions_val = DimensionsStruct::fromXmp(
      xmpData, "Xmp.mwg-rs.Regions/mwg-rs:AppliedToDimensions");
  std::vector<Region> regionList_val;

  // Parse RegionList
  int regionIndex = 1;
  while (true) {
    std::string baseKey = "Xmp.mwg-rs.Regions/mwg-rs:RegionList[" +
                          std::to_string(regionIndex) + "]";

    // Check if this region exists
    auto regionKey = xmpData.findKey(Exiv2::XmpKey(baseKey));
    if (regionKey == xmpData.end())
      break;

    regionList_val.push_back(Region::fromXmp(xmpData, baseKey));
    regionIndex++;
  }

  return RegionInfoStruct(appliedToDimensions_val, regionList_val);
}

void RegionInfoStruct::toXmp(Exiv2::XmpData &xmpData) const {
  LOG_DEBUG("Writing MWG Regions hierarchy");

  // Clear existing MWG Regions data
  clearXmpKey(xmpData, "Xmp.mwg-rs.Regions");

  // Write AppliedToDimensions first
  AppliedToDimensions.toXmp(xmpData,
                            "Xmp.mwg-rs.Regions/mwg-rs:AppliedToDimensions");

  const std::string baseRegionList = "Xmp.mwg-rs.Regions/mwg-rs:RegionList";
  xmpData[baseRegionList] =
      ""; // This might be needed for Exiv2 to establish the list

  // Write regions if any exist
  for (size_t i = 0; i < RegionList.size(); ++i) {
    const auto &region = RegionList[i];
    const std::string itemPath =
        baseRegionList + "[" + std::to_string(i + 1) + "]";

    region.toXmp(xmpData, itemPath);
  }

  LOG_DEBUG("Wrote " + std::to_string(RegionList.size()) + " regions.");
}

bool operator==(const RegionStruct &lhs, const RegionStruct &rhs) {
  return lhs.Area == rhs.Area && lhs.Name == rhs.Name && lhs.Type == rhs.Type &&
         lhs.Description == rhs.Description;
}

bool operator==(const RegionInfoStruct &lhs, const RegionInfoStruct &rhs) {
  return lhs.AppliedToDimensions == rhs.AppliedToDimensions &&
         lhs.RegionList == rhs.RegionList;
}
