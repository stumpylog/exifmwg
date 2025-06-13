#include <filesystem>
#include <stdexcept>

#include "Image.hpp"
#include "KeywordInfoModel.hpp"
#include "MetadataKeys.hpp"
#include "RegionInfoStruct.hpp"
#include "XmpUtils.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

// --- Constructor and save methods remain the same ---

Image::Image(const fs::path& path) : m_originalPath(path) {
  if (!fs::exists(m_originalPath) || !fs::is_regular_file(m_originalPath)) {
    throw std::runtime_error("File does not exist: " + path.string());
  }
  _readMetadata();
}

fs::path Image::getPath() const {
  return m_originalPath;
}

void Image::save(const std::optional<fs::path>& newPath) {
  fs::path targetPath = newPath.value_or(m_originalPath);

  // If saving to a new location, copy the original file there first.
  if (newPath && (m_originalPath != targetPath)) {
    try {
      fs::copy_file(m_originalPath, targetPath, fs::copy_options::overwrite_existing);
    } catch (const fs::filesystem_error& e) {
      throw std::runtime_error("Failed to copy file to new path: " + std::string(e.what()));
    }
  }

  _writeMetadata(targetPath);

  // If we saved to a new path, update the object's path to point to the new file.
  if (newPath) {
    m_originalPath = targetPath;
  }
}

void Image::clearMetadata() {
  _clearMetadataFields(m_originalPath);
}

// --- Metadata methods are now refactored ---

void Image::_readMetadata() {
  try {
    auto image = Exiv2::ImageFactory::open(m_originalPath.string());
    image->readMetadata();

    this->ImageHeight = image->pixelHeight();
    this->ImageWidth = image->pixelWidth();

    auto& exifData = image->exifData();
    auto& xmpData = image->xmpData();
    auto& iptcData = image->iptcData();

    // Orientation
    auto orientKey = exifData.findKey(Exiv2::ExifKey(MetadataKeys::Exif::Orientation));
    if (orientKey != exifData.end()) {
      this->Orientation = static_cast<int>(orientKey->toInt64());
    } else {
      this->Orientation = std::nullopt;
    }

    // Title and Description
    auto titleKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::Title));
    if (titleKey != xmpData.end()) {
      this->Title = XmpUtils::cleanXmpText(titleKey->toString());
    } else {
      this->Title = std::nullopt;
    }

    auto descKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::Description));
    if (descKey != xmpData.end()) {
      this->Description = XmpUtils::cleanXmpText(descKey->toString());
    } else {
      auto iptcDescKey = iptcData.findKey(Exiv2::IptcKey(MetadataKeys::Iptc::Caption));
      if (iptcDescKey != iptcData.end()) {
        this->Description = XmpUtils::cleanXmpText(iptcDescKey->toString());
      } else {
        this->Description = std::nullopt;
      }
    }

    // Location data - try IPTC first, then XMP fallback
    auto countryKey = iptcData.findKey(Exiv2::IptcKey(MetadataKeys::Iptc::CountryName));
    if (countryKey != iptcData.end()) {
      this->Country = countryKey->toString();
    } else {
      auto xmpCountryKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::IptcCountryName));
      if (xmpCountryKey != xmpData.end()) {
        this->Country = xmpCountryKey->toString();
      } else {
        this->Country = std::nullopt;
      }
    }

    auto cityKey = iptcData.findKey(Exiv2::IptcKey(MetadataKeys::Iptc::City));
    if (cityKey != iptcData.end()) {
      this->City = cityKey->toString();
    } else {
      auto xmpCityKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::PhotoshopCity));
      if (xmpCityKey != xmpData.end()) {
        this->City = xmpCityKey->toString();
      } else {
        this->City = std::nullopt;
      }
    }

    auto stateKey = iptcData.findKey(Exiv2::IptcKey(MetadataKeys::Iptc::ProvinceState));
    if (stateKey != iptcData.end()) {
      this->State = stateKey->toString();
    } else {
      auto xmpStateKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::PhotoshopState));
      if (xmpStateKey != xmpData.end()) {
        this->State = xmpStateKey->toString();
      } else {
        this->State = std::nullopt;
      }
    }

    auto locationKey = iptcData.findKey(Exiv2::IptcKey(MetadataKeys::Iptc::SubLocation));
    if (locationKey != iptcData.end()) {
      this->Location = locationKey->toString();
    } else {
      auto xmpLocationKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::IptcLocation));
      if (xmpLocationKey != xmpData.end()) {
        this->Location = xmpLocationKey->toString();
      } else {
        this->Location = std::nullopt;
      }
    }

    // Region Info
    auto regionInfoKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::Regions));
    if (regionInfoKey != xmpData.end()) {
      this->RegionInfo = RegionInfoStruct::fromXmp(xmpData);
    } else {
      this->RegionInfo = std::nullopt;
    }

    // Keywords
    auto lastKeywordXMP = XmpUtils::parseDelimitedString(xmpData, MetadataKeys::Xmp::MicrosoftLastKeywordXMP, ',');
    if (!lastKeywordXMP.empty()) {
      this->LastKeywordXMP = lastKeywordXMP;
    } else {
      this->LastKeywordXMP = std::nullopt;
    }

    auto tagsList = XmpUtils::parseDelimitedString(xmpData, MetadataKeys::Xmp::DigiKamTagsList, ',');
    if (!tagsList.empty()) {
      this->TagsList = tagsList;
    } else {
      this->TagsList = std::nullopt;
    }

    auto catalogSets = XmpUtils::parseDelimitedString(xmpData, MetadataKeys::Xmp::MediaProCatalogSets, ',');
    if (!catalogSets.empty()) {
      this->CatalogSets = catalogSets;
    } else {
      this->CatalogSets = std::nullopt;
    }

    auto hierarchicalSubject =
        XmpUtils::parseDelimitedString(xmpData, MetadataKeys::Xmp::LightroomHierarchicalSubject, ',');
    if (!hierarchicalSubject.empty()) {
      this->HierarchicalSubject = hierarchicalSubject;
    } else {
      this->HierarchicalSubject = std::nullopt; // Corrected a bug here that was setting CatalogSets to nullopt
    }

    this->KeywordInfo = KeywordInfoModel::fromXmp(xmpData);

  } catch (const Exiv2::Error& e) {
    throw std::runtime_error("Exiv2 error while reading: " + std::string(e.what()));
  }
}

void Image::_writeMetadata(const fs::path& targetPath) {
  try {
    auto image = Exiv2::ImageFactory::open(targetPath.string());
    image->readMetadata();

    auto& xmpData = image->xmpData();
    auto& exifData = image->exifData();
    auto& iptcData = image->iptcData();

    if (this->Title) {
      xmpData[MetadataKeys::Xmp::Title] = *this->Title;
    }
    if (this->Description) {
      xmpData[MetadataKeys::Xmp::Description] = *this->Description;
      iptcData[MetadataKeys::Iptc::Caption] = *this->Description;
    }
    if (this->Orientation) {
      exifData[MetadataKeys::Exif::Orientation] = *this->Orientation;
    }

    if (this->Country) {
      iptcData[MetadataKeys::Iptc::CountryName] = *this->Country;
      xmpData[MetadataKeys::Xmp::IptcCountryName] = *this->Country;
    }
    if (this->State) {
      iptcData[MetadataKeys::Iptc::ProvinceState] = *this->State;
      xmpData[MetadataKeys::Xmp::PhotoshopState] = *this->State;
    }
    if (this->City) {
      iptcData[MetadataKeys::Iptc::City] = *this->City;
      xmpData[MetadataKeys::Xmp::PhotoshopCity] = *this->City;
    }
    if (this->Location) {
      iptcData[MetadataKeys::Iptc::SubLocation] = *this->Location;
      xmpData[MetadataKeys::Xmp::IptcLocation] = *this->Location;
    }

    if (this->RegionInfo) {
      this->RegionInfo.value().toXmp(xmpData);
    }

    if (this->KeywordInfo) {
      this->KeywordInfo.value().toXmp(xmpData);
    }

    if (this->LastKeywordXMP) {
      XmpUtils::clearXmpKey(xmpData, MetadataKeys::Xmp::MicrosoftLastKeywordXMP);
      std::string combined = "";
      for (size_t i = 0; i < this->LastKeywordXMP->size(); ++i) {
        if (i > 0)
          combined += ",";
        combined += (*this->LastKeywordXMP)[i];
      }
      xmpData[MetadataKeys::Xmp::MicrosoftLastKeywordXMP] = combined;
    }

    if (this->TagsList) {
      XmpUtils::clearXmpKey(xmpData, MetadataKeys::Xmp::DigiKamTagsList);
      std::string combined = "";
      for (size_t i = 0; i < this->TagsList->size(); ++i) {
        if (i > 0)
          combined += ",";
        combined += (*this->TagsList)[i];
      }
      xmpData[MetadataKeys::Xmp::DigiKamTagsList] = combined;
    }

    if (this->CatalogSets) {
      XmpUtils::clearXmpKey(xmpData, MetadataKeys::Xmp::MediaProCatalogSets);
      std::string combined = "";
      for (size_t i = 0; i < this->CatalogSets->size(); ++i) {
        if (i > 0)
          combined += ",";
        combined += (*this->CatalogSets)[i];
      }
      xmpData[MetadataKeys::Xmp::MediaProCatalogSets] = combined;
    }

    if (this->HierarchicalSubject) {
      XmpUtils::clearXmpKey(xmpData, MetadataKeys::Xmp::LightroomHierarchicalSubject);
      std::string combined = "";
      for (size_t i = 0; i < this->HierarchicalSubject->size(); ++i) {
        if (i > 0)
          combined += ",";
        combined += (*this->HierarchicalSubject)[i];
      }
      xmpData[MetadataKeys::Xmp::LightroomHierarchicalSubject] = combined;
    }

    image->writeMetadata();
  } catch (const Exiv2::Error& e) {
    throw std::runtime_error("Exiv2 error while writing: " + std::string(e.what()));
  }
}

void Image::_clearMetadataFields(const fs::path& targetPath) {
  try {
    auto image = Exiv2::ImageFactory::open(targetPath.string());
    image->readMetadata();

    auto& xmpData = image->xmpData();
    auto& exifData = image->exifData();
    auto& iptcData = image->iptcData();

    // Helper lambda to remove keys matching a pattern
    auto removeKeysMatching = [](auto& data, const std::string& pattern) {
      auto it = data.begin();
      LOG_DEBUG("Checking pattern " + pattern);
      while (it != data.end()) {
        if (it->key().find(pattern) != std::string::npos) {
          it = data.erase(it);
          LOG_DEBUG("Erasing");
        } else {
          ++it;
        }
      }
      LOG_DEBUG("pattern " + pattern + " done");
    };

    // Clear face regions
    removeKeysMatching(xmpData, "Xmp.mwg-rs.Regions");

    // Clear orientation
    auto orientIt = exifData.findKey(Exiv2::ExifKey(MetadataKeys::Exif::Orientation));
    if (orientIt != exifData.end()) {
      exifData.erase(orientIt);
    }

    // Clear keyword info and hierarchical keywords
    auto xmpKeywordIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::Keywords));
    if (xmpKeywordIt != xmpData.end()) {
      xmpData.erase(xmpKeywordIt);
    }
    xmpKeywordIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::KeywordInfo));
    if (xmpKeywordIt != xmpData.end()) {
      xmpData.erase(xmpKeywordIt);
    }
    xmpKeywordIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::AcdseeCategories));
    if (xmpKeywordIt != xmpData.end()) {
      xmpData.erase(xmpKeywordIt);
    }
    xmpKeywordIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::MicrosoftLastKeywordXMP));
    if (xmpKeywordIt != xmpData.end()) {
      xmpData.erase(xmpKeywordIt);
    }
    xmpKeywordIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::DigiKamTagsList));
    if (xmpKeywordIt != xmpData.end()) {
      xmpData.erase(xmpKeywordIt);
    }
    xmpKeywordIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::LightroomHierarchicalSubject));
    if (xmpKeywordIt != xmpData.end()) {
      xmpData.erase(xmpKeywordIt);
    }
    xmpKeywordIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::MediaProCatalogSets));
    if (xmpKeywordIt != xmpData.end()) {
      xmpData.erase(xmpKeywordIt);
    }

    // Clear titles and descriptions from multiple sources
    // XMP
    auto titleIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::Title));
    if (titleIt != xmpData.end()) {
      xmpData.erase(titleIt);
    }

    auto descIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::Description));
    if (descIt != xmpData.end()) {
      xmpData.erase(descIt);
    }
    // IPTC
    auto iptcCaptionIt = iptcData.findKey(Exiv2::IptcKey(MetadataKeys::Iptc::Caption));
    if (iptcCaptionIt != iptcData.end()) {
      iptcData.erase(iptcCaptionIt);
    }

    // EXIF
    auto exifDescIt = exifData.findKey(Exiv2::ExifKey(MetadataKeys::Exif::ImageDescription));
    if (exifDescIt != exifData.end()) {
      exifData.erase(exifDescIt);
    }

    image->writeMetadata();

    // After writing the cleared metadata, re-read to update object state.
    _readMetadata();

  } catch (const Exiv2::Error& e) {
    throw std::runtime_error("Exiv2 error while clearing: " + std::string(e.what()));
  }
}
/**
 * @brief Checks equality for two Image objects.
 *
 * Compares all metadata fields between two Image instances for equality.
 *
 * @param lhs The left-hand side Image object.
 * @param rhs The right-hand side Image object.
 * @return true if all fields are equal; false otherwise.
 */
bool operator==(const Image& lhs, const Image& rhs) {
  return (lhs.ImageHeight == rhs.ImageHeight) && (lhs.ImageWidth == rhs.ImageWidth) && (lhs.Title == rhs.Title) &&
         (lhs.Description == rhs.Description) && (lhs.RegionInfo == rhs.RegionInfo) &&
         (lhs.Orientation == rhs.Orientation) && (lhs.LastKeywordXMP == rhs.LastKeywordXMP) &&
         (lhs.TagsList == rhs.TagsList) && (lhs.CatalogSets == rhs.CatalogSets) &&
         (lhs.HierarchicalSubject == rhs.HierarchicalSubject) && (lhs.KeywordInfo == rhs.KeywordInfo) &&
         (lhs.Country == rhs.Country) && (lhs.City == rhs.City) && (lhs.State == rhs.State) &&
         (lhs.Location == rhs.Location);
}

/**
 * @brief Checks inequality for two Image objects.
 *
 * Inverse of the equality operator; returns true if any field differs between the two objects.
 *
 * @param lhs The left-hand side Image object.
 * @param rhs The right-hand side Image object.
 * @return true if any field differs; false otherwise.
 */
bool operator!=(const Image& lhs, const Image& rhs) {
  return !(lhs == rhs);
}
