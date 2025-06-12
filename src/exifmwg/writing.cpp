#include <string>

#include "exiv2/exiv2.hpp"

#include "ImageMetadata.hpp"
#include "XmpUtils.hpp"
#include "clearing.hpp"
#include "utils.hpp"
#include "writing.hpp"

#include "KeywordInfoModel.hpp"

namespace fs = std::filesystem;

void write_metadata(const fs::path& filepath, const ImageMetadata& metadata) {
  try {
    auto image = Exiv2::ImageFactory::open(filepath.string());
    if (!image.get()) {
      throw std::runtime_error("Cannot open file: " + filepath.string());
    }

    image->readMetadata();
    auto& xmpData = image->xmpData();
    auto& exifData = image->exifData();
    auto& iptcData = image->iptcData();

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
      metadata.RegionInfo.value().toXmp(xmpData);
    }

    // Write MWG Keywords
    if (metadata.KeywordInfo) {
      metadata.KeywordInfo.value().toXmp(xmpData);
    }

    // Write keyword arrays as comma-delimited strings
    if (metadata.LastKeywordXMP) {
      XmpUtils::clearXmpKey(xmpData, "Xmp.MicrosoftPhoto.LastKeywordXMP");
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
      XmpUtils::clearXmpKey(xmpData, "Xmp.digiKam.TagsList");

      std::string combined = "";
      for (size_t i = 0; i < metadata.TagsList->size(); ++i) {
        if (i > 0)
          combined += ",";
        combined += (*metadata.TagsList)[i];
      }
      xmpData["Xmp.digiKam.TagsList"] = combined;
    }

    if (metadata.CatalogSets) {
      XmpUtils::clearXmpKey(xmpData, "Xmp.mediapro.CatalogSets");
      std::string combined = "";
      for (size_t i = 0; i < metadata.CatalogSets->size(); ++i) {
        if (i > 0)
          combined += ",";
        combined += (*metadata.CatalogSets)[i];
      }
      xmpData["Xmp.mediapro.CatalogSets"] = combined;
    }

    if (metadata.HierarchicalSubject) {
      XmpUtils::clearXmpKey(xmpData, "Xmp.lr.hierarchicalSubject");

      std::string combined = "";
      for (size_t i = 0; i < metadata.HierarchicalSubject->size(); ++i) {
        if (i > 0)
          combined += ",";
        combined += (*metadata.HierarchicalSubject)[i];
      }
      xmpData["Xmp.lr.hierarchicalSubject"] = combined;
    }

    image->writeMetadata();
  } catch (const Exiv2::Error& e) {
    throw std::runtime_error("Exiv2 error: " + std::string(e.what()));
  }
}
