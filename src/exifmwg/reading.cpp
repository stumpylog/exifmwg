
#include "exiv2/exiv2.hpp"

#include "reading.hpp"
#include "utils.hpp"

#include "KeywordInfoModel.hpp"
#include "RegionInfoStruct.hpp"
#include "XmpAreaStruct.hpp"
#include "XmpUtils.hpp"

namespace fs = std::filesystem;

ImageMetadata read_metadata(const fs::path &filepath) {
  try {
    auto image = Exiv2::ImageFactory::open(filepath.string());
    if (!image.get()) {
      throw std::runtime_error("Cannot open file: " + filepath.string());
    }

    image->readMetadata();

    auto &exifData = image->exifData();
    auto &xmpData = image->xmpData();
    auto &iptcData = image->iptcData();

    // Basic image dimensions
    // TODO: Consider "Exif.Photo.PixelXDimension" and
    // "Exif.Image.ImageWidth" (and the equivalents Y) for something
    ImageMetadata metadata(image->pixelHeight(), image->pixelWidth());

    // Orientation
    auto orientKey = exifData.findKey(Exiv2::ExifKey("Exif.Image.Orientation"));
    if (orientKey != exifData.end()) {
      metadata.Orientation = static_cast<int>(orientKey->toInt64());
    }

    // Title and Description
    auto titleKey = xmpData.findKey(Exiv2::XmpKey("Xmp.dc.title"));
    if (titleKey != xmpData.end()) {
      metadata.Title = XmpUtils::cleanXmpText(titleKey->toString());
    }

    auto descKey = xmpData.findKey(Exiv2::XmpKey("Xmp.dc.description"));
    if (descKey != xmpData.end()) {
      metadata.Description = XmpUtils::cleanXmpText(descKey->toString());
    }

    // Location data - try IPTC first, then XMP fallback
    auto countryKey =
        iptcData.findKey(Exiv2::IptcKey("Iptc.Application2.CountryName"));
    if (countryKey != iptcData.end()) {
      metadata.Country = countryKey->toString();
    } else {
      auto xmpCountryKey =
          xmpData.findKey(Exiv2::XmpKey("Xmp.iptc.CountryName"));
      if (xmpCountryKey != xmpData.end()) {
        metadata.Country = xmpCountryKey->toString();
      }
    }

    auto cityKey = iptcData.findKey(Exiv2::IptcKey("Iptc.Application2.City"));
    if (cityKey != iptcData.end()) {
      metadata.City = cityKey->toString();
    } else {
      auto xmpCityKey = xmpData.findKey(Exiv2::XmpKey("Xmp.photoshop.City"));
      if (xmpCityKey != xmpData.end()) {
        metadata.City = xmpCityKey->toString();
      }
    }

    auto stateKey =
        iptcData.findKey(Exiv2::IptcKey("Iptc.Application2.ProvinceState"));
    if (stateKey != iptcData.end()) {
      metadata.State = stateKey->toString();
    } else {
      auto xmpStateKey = xmpData.findKey(Exiv2::XmpKey("Xmp.photoshop.State"));
      if (xmpStateKey != xmpData.end()) {
        metadata.State = xmpStateKey->toString();
      }
    }

    auto locationKey =
        iptcData.findKey(Exiv2::IptcKey("Iptc.Application2.SubLocation"));
    if (locationKey != iptcData.end()) {
      metadata.Location = locationKey->toString();
    } else {
      auto xmpLocationKey = xmpData.findKey(Exiv2::XmpKey("Xmp.iptc.Location"));
      if (xmpLocationKey != xmpData.end()) {
        metadata.Location = xmpLocationKey->toString();
      }
    }

    // Region Info
    if (!xmpData.empty()) {
      metadata.RegionInfo = RegionInfoStruct::fromXmp(xmpData);
    }

    // Keywords
    auto lastKeywordXMP = XmpUtils::parseDelimitedString(
        xmpData, "Xmp.MicrosoftPhoto.LastKeywordXMP", ',');
    if (!lastKeywordXMP.empty()) {
      metadata.LastKeywordXMP = lastKeywordXMP;
    }

    auto tagsList =
        XmpUtils::parseDelimitedString(xmpData, "Xmp.digiKam.TagsList", ',');
    if (!tagsList.empty()) {
      metadata.TagsList = tagsList;
    }

    auto catalogSets = XmpUtils::parseDelimitedString(
        xmpData, "Xmp.mediapro.CatalogSets", ',');
    if (!catalogSets.empty()) {
      metadata.CatalogSets = catalogSets;
    }

    auto hierarchicalSubject = XmpUtils::parseDelimitedString(
        xmpData, "Xmp.lr.hierarchicalSubject", ',');
    if (!hierarchicalSubject.empty()) {
      metadata.HierarchicalSubject = hierarchicalSubject;
    }
    metadata.KeywordInfo = KeywordInfoModel::fromXmp(xmpData);

    return metadata;

  } catch (const Exiv2::Error &e) {
    throw std::runtime_error("Exiv2 error: " + std::string(e.what()));
  }
}
