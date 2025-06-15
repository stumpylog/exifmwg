#include "ImageMetadata.hpp"

#include "MetadataKeys.hpp"

namespace fs = fs;

/**
 * @brief Constructs an ImageMetadata object with various optional metadata fields.
 *
 * This constructor initializes all metadata fields for an image, including dimensions,
 * descriptive fields, region info, orientation, keyword lists, location data, and more.
 *
 * @param imageHeight The height of the image in pixels.
 * @param imageWidth The width of the image in pixels.
 * @param title Optional image title.
 * @param description Optional image description.
 * @param regionInfo Optional region information struct.
 * @param orientation Optional EXIF orientation value.
 * @param lastKeywordXMP Optional vector of last XMP keywords.
 * @param tagsList Optional vector of tags.
 * @param catalogSets Optional vector of catalog set names.
 * @param hierarchicalSubject Optional vector of hierarchical subject strings.
 * @param keywordInfo Optional keyword info model.
 * @param country Optional country string.
 * @param city Optional city string.
 * @param state Optional state string.
 * @param location Optional location string.
 */
ImageMetadata::ImageMetadata(int imageHeight, int imageWidth, std::optional<std::string> title,
                             std::optional<std::string> description, std::optional<RegionInfoStruct> regionInfo,
                             std::optional<int> orientation, std::optional<std::vector<std::string>> lastKeywordXMP,
                             std::optional<std::vector<std::string>> tagsList,
                             std::optional<std::vector<std::string>> catalogSets,
                             std::optional<std::vector<std::string>> hierarchicalSubject,
                             std::optional<KeywordInfoModel> keywordInfo, std::optional<std::string> country,
                             std::optional<std::string> city, std::optional<std::string> state,
                             std::optional<std::string> location) :
    ImageHeight(imageHeight), ImageWidth(imageWidth), Title(title), Description(description), RegionInfo(regionInfo),
    Orientation(orientation), LastKeywordXMP(lastKeywordXMP), TagsList(tagsList), CatalogSets(catalogSets),
    HierarchicalSubject(hierarchicalSubject), KeywordInfo(keywordInfo), Country(country), City(city), State(state),
    Location(location) {
}

static ImageMetadata fromFile(const fs::path& path) {
  if (!fs::exists(path) || !fs::is_regular_file(path)) {
    throw std::runtime_error("File does not exist: " + path.string());
  }
  try {
    auto image = Exiv2::ImageFactory::open(m_originalPath.string());
    image->readMetadata();

    ImageMetadata metadata(image->pixelHeight(), image->pixelWidth());

    auto& exifData = image->exifData();
    auto& xmpData = image->xmpData();
    auto& iptcData = image->iptcData();

    // Orientation
    auto orientKey = exifData.findKey(Exiv2::ExifKey(MetadataKeys::Exif::Orientation));
    if (orientKey != exifData.end()) {
      metadata.Orientation = static_cast<int>(orientKey->toInt64());
    } else {
      metadata.Orientation = std::nullopt;
    }

    // Title and Description
    auto titleKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::Title));
    if (titleKey != xmpData.end()) {
      metadata.Title = XmpUtils::cleanXmpText(titleKey->toString());
    } else {
      metadata.Title = std::nullopt;
    }

    auto descKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::Description));
    if (descKey != xmpData.end()) {
      metadata.Description = XmpUtils::cleanXmpText(descKey->toString());
    } else {
      auto iptcDescKey = iptcData.findKey(Exiv2::IptcKey(MetadataKeys::Iptc::Caption));
      if (iptcDescKey != iptcData.end()) {
        metadata.Description = XmpUtils::cleanXmpText(iptcDescKey->toString());
      } else {
        metadata.Description = std::nullopt;
      }
    }

    // Location data - try IPTC first, then XMP fallback
    auto countryKey = iptcData.findKey(Exiv2::IptcKey(MetadataKeys::Iptc::CountryName));
    if (countryKey != iptcData.end()) {
      metadata.Country = countryKey->toString();
    } else {
      auto xmpCountryKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::IptcCountryName));
      if (xmpCountryKey != xmpData.end()) {
        metadata.Country = xmpCountryKey->toString();
      } else {
        metadata.Country = std::nullopt;
      }
    }

    auto cityKey = iptcData.findKey(Exiv2::IptcKey(MetadataKeys::Iptc::City));
    if (cityKey != iptcData.end()) {
      metadata.City = cityKey->toString();
    } else {
      auto xmpCityKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::PhotoshopCity));
      if (xmpCityKey != xmpData.end()) {
        metadata.City = xmpCityKey->toString();
      } else {
        metadata.City = std::nullopt;
      }
    }

    auto stateKey = iptcData.findKey(Exiv2::IptcKey(MetadataKeys::Iptc::ProvinceState));
    if (stateKey != iptcData.end()) {
      metadata.State = stateKey->toString();
    } else {
      auto xmpStateKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::PhotoshopState));
      if (xmpStateKey != xmpData.end()) {
        metadata.State = xmpStateKey->toString();
      } else {
        metadata.State = std::nullopt;
      }
    }

    auto locationKey = iptcData.findKey(Exiv2::IptcKey(MetadataKeys::Iptc::SubLocation));
    if (locationKey != iptcData.end()) {
      metadata.Location = locationKey->toString();
    } else {
      auto xmpLocationKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::IptcLocation));
      if (xmpLocationKey != xmpData.end()) {
        metadata.Location = xmpLocationKey->toString();
      } else {
        metadata.Location = std::nullopt;
      }
    }

    // Region Info
    auto regionInfoKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::Regions));
    if (regionInfoKey != xmpData.end()) {
      metadata.RegionInfo = RegionInfoStruct::fromXmp(xmpData);
    } else {
      metadata.RegionInfo = std::nullopt;
    }

    // Keywords
    auto lastKeywordXMP = XmpUtils::parseDelimitedString(xmpData, MetadataKeys::Xmp::MicrosoftLastKeywordXMP, ',');
    if (!lastKeywordXMP.empty()) {
      metadata.LastKeywordXMP = lastKeywordXMP;
    } else {
      metadata.LastKeywordXMP = std::nullopt;
    }

    auto tagsList = XmpUtils::parseDelimitedString(xmpData, MetadataKeys::Xmp::DigiKamTagsList, ',');
    if (!tagsList.empty()) {
      metadata.TagsList = tagsList;
    } else {
      metadata.TagsList = std::nullopt;
    }

    auto catalogSets = XmpUtils::parseDelimitedString(xmpData, MetadataKeys::Xmp::MediaProCatalogSets, ',');
    if (!catalogSets.empty()) {
      metadata.CatalogSets = catalogSets;
    } else {
      metadata.CatalogSets = std::nullopt;
    }

    auto hierarchicalSubject =
        XmpUtils::parseDelimitedString(xmpData, MetadataKeys::Xmp::LightroomHierarchicalSubject, ',');
    if (!hierarchicalSubject.empty()) {
      metadata.HierarchicalSubject = hierarchicalSubject;
    } else {
      metadata.HierarchicalSubject = std::nullopt;
    }

    metadata.KeywordInfo = KeywordInfoModel::fromXmp(xmpData);

    return metadata;

  } catch (const Exiv2::Error& e) {
    throw std::runtime_error("Exiv2 error while reading: " + std::string(e.what()));
  }
}
void toFile(const std::optional<fs::path>& newPath = std::nullopt) {
  fs::path targetPath = this->m_originalPath;
  if (newPath) {
    targetPath = newPath;
  }

  if (this->m_originalPath != targetPath) {
    try {
      fs::copy_file(this->m_originalPath, targetPath, fs::copy_options::overwrite_existing);
    } catch (const fs::filesystem_error& e) {
      throw std::runtime_error("Failed to copy file to new path: " + std::string(e.what()));
    }
  }

  try {
    auto image = Exiv2::ImageFactory::open(targetPath.string());

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

static void clearFile(const fs::path& path) {
  try {
    auto image = Exiv2::ImageFactory::open(path.string());
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
 * @brief Checks equality for two ImageMetadata objects.
 *
 * Compares all metadata fields between two ImageMetadata instances for equality.
 *
 * @param lhs The left-hand side ImageMetadata object.
 * @param rhs The right-hand side ImageMetadata object.
 * @return true if all fields are equal; false otherwise.
 */
bool operator==(const ImageMetadata& lhs, const ImageMetadata& rhs) {
  return (lhs.ImageHeight == rhs.ImageHeight) && (lhs.ImageWidth == rhs.ImageWidth) && (lhs.Title == rhs.Title) &&
         (lhs.Description == rhs.Description) && (lhs.RegionInfo == rhs.RegionInfo) &&
         (lhs.Orientation == rhs.Orientation) && (lhs.LastKeywordXMP == rhs.LastKeywordXMP) &&
         (lhs.TagsList == rhs.TagsList) && (lhs.CatalogSets == rhs.CatalogSets) &&
         (lhs.HierarchicalSubject == rhs.HierarchicalSubject) && (lhs.KeywordInfo == rhs.KeywordInfo) &&
         (lhs.Country == rhs.Country) && (lhs.City == rhs.City) && (lhs.State == rhs.State) &&
         (lhs.Location == rhs.Location);
}

/**
 * @brief Checks inequality for two ImageMetadata objects.
 *
 * Inverse of the equality operator; returns true if any field differs between the two objects.
 *
 * @param lhs The left-hand side ImageMetadata object.
 * @param rhs The right-hand side ImageMetadata object.
 * @return true if any field differs; false otherwise.
 */
bool operator!=(const ImageMetadata& lhs, const ImageMetadata& rhs) {
  return !(lhs == rhs);
}
