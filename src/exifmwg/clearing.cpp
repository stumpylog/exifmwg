#include "clearing.hpp"

#include <filesystem>

#include "exiv2/exiv2.hpp"

namespace fs = std::filesystem;

void clearXmpKey(Exiv2::XmpData &xmpData, const std::string &baseKey) {
  auto it = xmpData.begin();
  while (it != xmpData.end()) {
    if (it->key().find(baseKey) != std::string::npos) {
      it = xmpData.erase(it);
    } else {
      ++it;
    }
  }
}

void clear_existing_metadata(const fs::path &filepath) {
  try {
    auto image = Exiv2::ImageFactory::open(filepath.string());
    if (!image.get()) {
      throw std::runtime_error("Cannot open file: " + filepath.string());
    }

    image->readMetadata();
    auto &xmpData = image->xmpData();
    auto &exifData = image->exifData();
    auto &iptcData = image->iptcData();

    // Helper lambda to remove keys matching a pattern
    auto removeKeysMatching = [](auto &data, const std::string &pattern) {
      auto it = data.begin();
      while (it != data.end()) {
        if (it->key().find(pattern) != std::string::npos) {
          it = data.erase(it);
        } else {
          ++it;
        }
      }
    };

    // Clear face regions
    removeKeysMatching(xmpData, "Xmp.mwg-rs.Regions");

    // Clear orientation
    auto orientIt = exifData.findKey(Exiv2::ExifKey("Exif.Image.Orientation"));
    if (orientIt != exifData.end()) {
      exifData.erase(orientIt);
    }

    // Clear keyword info and hierarchical keywords
    removeKeysMatching(xmpData, "Xmp.mwg-kw.KeywordInfo");
    removeKeysMatching(xmpData, "Xmp.acdsee.Categories");
    removeKeysMatching(xmpData, "Xmp.microsoft.LastKeywordXMP");
    removeKeysMatching(xmpData, "Xmp.digiKam.TagsList");
    removeKeysMatching(xmpData, "Xmp.lr.hierarchicalSubject");
    removeKeysMatching(xmpData, "Xmp.mediapro.CatalogSets");

    // Clear titles and descriptions from multiple sources
    // XMP
    auto titleIt = xmpData.findKey(Exiv2::XmpKey("Xmp.dc.title"));
    if (titleIt != xmpData.end()) {
      xmpData.erase(titleIt);
    }

    auto descIt = xmpData.findKey(Exiv2::XmpKey("Xmp.dc.description"));
    if (descIt != xmpData.end()) {
      xmpData.erase(descIt);
    }

    // EXIF
    auto exifDescIt =
        exifData.findKey(Exiv2::ExifKey("Exif.Image.ImageDescription"));
    if (exifDescIt != exifData.end()) {
      exifData.erase(exifDescIt);
    }

    auto ifd0DescIt =
        exifData.findKey(Exiv2::ExifKey("Exif.Image.ImageDescription"));
    if (ifd0DescIt != exifData.end()) {
      exifData.erase(ifd0DescIt);
    }

    // IPTC
    auto iptcCaptionIt =
        iptcData.findKey(Exiv2::IptcKey("Iptc.Application2.Caption"));
    if (iptcCaptionIt != iptcData.end()) {
      iptcData.erase(iptcCaptionIt);
    }

    image->writeMetadata();
  } catch (const Exiv2::Error &e) {
    throw std::runtime_error("Exiv2 error: " + std::string(e.what()));
  }
}
