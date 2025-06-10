#include <string>

#include "exiv2/exiv2.hpp"

#include "clearing.hpp"
#include "models.hpp"
#include "utils.hpp"
#include "writing.hpp"

#include "KeywordInfoModel.hpp"

namespace fs = std::filesystem;

/**
 * @brief Writes the complete MWG Regions structure to XMP metadata.
 *
 * @param xmpData The XMP data object to write to.
 * @param regionInfo The RegionInfoStruct containing the region data.
 */
void write_region_info(Exiv2::XmpData &xmpData,
                       const RegionInfoStruct &regionInfo) {
  LOG_DEBUG("Writing MWG Regions hierarchy");

  // Clear existing MWG Regions data
  clearXmpKey(xmpData, "Xmp.mwg-rs.Regions");

  // Write AppliedToDimensions first
  regionInfo.AppliedToDimensions.toXmp(
      xmpData, "Xmp.mwg-rs.Regions/mwg-rs:AppliedToDimensions");

  const std::string baseRegionList = "Xmp.mwg-rs.Regions/mwg-rs:RegionList";
  xmpData[baseRegionList] = "";

  // Write regions if any exist
  for (size_t i = 0; i < regionInfo.RegionList.size(); ++i) {
    const auto &region = regionInfo.RegionList[i];
    const std::string itemPath =
        baseRegionList + "[" + std::to_string(i + 1) + "]";

    // Write the Area struct
    std::string areaPath = itemPath + "/mwg-rs:Area";
    region.Area.toXmp(xmpData, areaPath);

    // Write other region properties
    xmpData[itemPath + "/mwg-rs:Name"] = region.Name;
    xmpData[itemPath + "/mwg-rs:Type"] = region.Type;

    if (region.Description) {
      xmpData[itemPath + "/mwg-rs:Description"] = *region.Description;
    }
  }

  LOG_DEBUG("Wrote " + std::to_string(regionInfo.RegionList.size()) +
            " regions.");
}

void write_metadata(const fs::path &filepath, const ImageMetadata &metadata) {
  try {
    auto image = Exiv2::ImageFactory::open(filepath.string());
    if (!image.get()) {
      throw std::runtime_error("Cannot open file: " + filepath.string());
    }

    image->readMetadata();
    auto &xmpData = image->xmpData();
    auto &exifData = image->exifData();
    auto &iptcData = image->iptcData();

    // Write basic fields
    if (metadata.Title) {
      xmpData["Xmp.dc.title"] = *metadata.Title;
    }

    if (metadata.Description) {
      xmpData["Xmp.dc.description"] = *metadata.Description;
    }

    if (metadata.Orientation) {
      exifData["Exif.Image.Orientation"] = *metadata.Orientation;
    }

    // Write location data to both IPTC and XMP for maximum compatibility
    if (metadata.Country) {
      iptcData["Iptc.Application2.CountryName"] = *metadata.Country;
      xmpData["Xmp.iptc.CountryName"] = *metadata.Country;
    }
    if (metadata.State) {
      iptcData["Iptc.Application2.ProvinceState"] = *metadata.State;
      xmpData["Xmp.photoshop.State"] = *metadata.State;
    }
    if (metadata.City) {
      iptcData["Iptc.Application2.City"] = *metadata.City;
      xmpData["Xmp.photoshop.City"] = *metadata.City;
    }
    if (metadata.Location) {
      iptcData["Iptc.Application2.SubLocation"] = *metadata.Location;
      xmpData["Xmp.iptc.Location"] = *metadata.Location;
    }

    // Write MWG Regions
    if (metadata.RegionInfo) {
      write_region_info(xmpData, *metadata.RegionInfo);
    }

    // Write MWG Keywords
    if (metadata.KeywordInfo) {
      metadata.KeywordInfo.value().toXmp(xmpData);
    }

    // Write keyword arrays as comma-delimited strings
    if (metadata.LastKeywordXMP) {
      clearXmpKey(xmpData, "Xmp.MicrosoftPhoto.LastKeywordXMP");
      // Write as comma-delimited string
      std::string combined = "";
      for (size_t i = 0; i < metadata.LastKeywordXMP->size(); ++i) {
        if (i > 0)
          combined += ",";
        combined += (*metadata.LastKeywordXMP)[i];
      }
      xmpData["Xmp.MicrosoftPhoto.LastKeywordXMP"] = combined;
    }

    if (metadata.TagsList) {
      clearXmpKey(xmpData, "Xmp.digiKam.TagsList");

      std::string combined = "";
      for (size_t i = 0; i < metadata.TagsList->size(); ++i) {
        if (i > 0)
          combined += ",";
        combined += (*metadata.TagsList)[i];
      }
      xmpData["Xmp.digiKam.TagsList"] = combined;
    }

    if (metadata.CatalogSets) {
      clearXmpKey(xmpData, "Xmp.mediapro.CatalogSets");
      std::string combined = "";
      for (size_t i = 0; i < metadata.CatalogSets->size(); ++i) {
        if (i > 0)
          combined += ",";
        combined += (*metadata.CatalogSets)[i];
      }
      xmpData["Xmp.mediapro.CatalogSets"] = combined;
    }

    if (metadata.HierarchicalSubject) {
      clearXmpKey(xmpData, "Xmp.lr.hierarchicalSubject");

      std::string combined = "";
      for (size_t i = 0; i < metadata.HierarchicalSubject->size(); ++i) {
        if (i > 0)
          combined += ",";
        combined += (*metadata.HierarchicalSubject)[i];
      }
      xmpData["Xmp.lr.hierarchicalSubject"] = combined;
    }

    image->writeMetadata();
  } catch (const Exiv2::Error &e) {
    throw std::runtime_error("Exiv2 error: " + std::string(e.what()));
  }
}
