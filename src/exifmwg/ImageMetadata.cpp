#include <utility>

#include "ImageMetadata.hpp"
#include "Logging.hpp"
#include "MetadataKeys.hpp"
#include "XmpUtils.hpp"

namespace fs = std::filesystem;

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
    ImageHeight(imageHeight), ImageWidth(imageWidth), Title(std::move(title)), Description(std::move(description)),
    RegionInfo(std::move(regionInfo)), Orientation(orientation), LastKeywordXMP(std::move(lastKeywordXMP)),
    TagsList(std::move(tagsList)), CatalogSets(std::move(catalogSets)),
    HierarchicalSubject(std::move(hierarchicalSubject)), KeywordInfo(std::move(keywordInfo)),
    Country(std::move(country)), City(std::move(city)), State(std::move(state)), Location(std::move(location)) {
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
ImageMetadata::ImageMetadata(const fs::path& path) {
  this->m_originalPath = path;
  if (!fs::exists(path) || !fs::is_regular_file(path)) {
    throw std::runtime_error("File does not exist: " + path.string());
  }
  try {
    auto image = Exiv2::ImageFactory::open(path.string());
    image->readMetadata();

    auto& exifData = image->exifData();
    auto& xmpData = image->xmpData();
    auto& iptcData = image->iptcData();

    this->ImageHeight = image->pixelHeight();
    this->ImageWidth = image->pixelWidth();

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
      this->HierarchicalSubject = std::nullopt;
    }

    this->KeywordInfo = KeywordInfoModel::fromXmp(xmpData);

  } catch (const Exiv2::Error& e) {
    throw std::runtime_error("Exiv2 error while reading: " + std::string(e.what()));
  }
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void ImageMetadata::toFile(const std::optional<fs::path>& newPath) {
  fs::path targetPath;
  if (newPath.has_value()) {
    targetPath = newPath.value();
  } else if (this->m_originalPath.has_value()) {
    targetPath = this->m_originalPath.value();
  } else {
    throw std::runtime_error("Unable to determine the target path");
  }

  if (this->m_originalPath.has_value() && (this->m_originalPath.value() != targetPath)) {
    try {
      fs::copy_file(this->m_originalPath.value(), targetPath, fs::copy_options::overwrite_existing);
    } catch (const fs::filesystem_error& e) {
      throw std::runtime_error("Failed to copy file to new path: " + std::string(e.what()));
    }
  }

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
      std::string combined;
      for (size_t i = 0; i < this->LastKeywordXMP->size(); ++i) {
        if (i > 0) {
          combined += ",";
        }
        combined += (*this->LastKeywordXMP)[i];
      }
      xmpData[MetadataKeys::Xmp::MicrosoftLastKeywordXMP] = combined;
    }

    if (this->TagsList) {
      XmpUtils::clearXmpKey(xmpData, MetadataKeys::Xmp::DigiKamTagsList);
      std::string combined;
      for (size_t i = 0; i < this->TagsList->size(); ++i) {
        if (i > 0) {
          combined += ",";
        }
        combined += (*this->TagsList)[i];
      }
      xmpData[MetadataKeys::Xmp::DigiKamTagsList] = combined;
    }

    if (this->CatalogSets) {
      XmpUtils::clearXmpKey(xmpData, MetadataKeys::Xmp::MediaProCatalogSets);
      std::string combined;
      for (size_t i = 0; i < this->CatalogSets->size(); ++i) {
        if (i > 0) {
          combined += ",";
        }
        combined += (*this->CatalogSets)[i];
      }
      xmpData[MetadataKeys::Xmp::MediaProCatalogSets] = combined;
    }

    if (this->HierarchicalSubject) {
      XmpUtils::clearXmpKey(xmpData, MetadataKeys::Xmp::LightroomHierarchicalSubject);
      std::string combined;
      for (size_t i = 0; i < this->HierarchicalSubject->size(); ++i) {
        if (i > 0) {
          combined += ",";
        }
        combined += (*this->HierarchicalSubject)[i];
      }
      xmpData[MetadataKeys::Xmp::LightroomHierarchicalSubject] = combined;
    }

    image->writeMetadata();
  } catch (const Exiv2::Error& e) {
    throw std::runtime_error("Exiv2 error while writing: " + std::string(e.what()));
  }
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void ImageMetadata::clearFile(const fs::path& path) {
  try {
    auto image = Exiv2::ImageFactory::open(path.string());
    image->readMetadata();

    auto& xmpData = image->xmpData();
    auto& exifData = image->exifData();
    auto& iptcData = image->iptcData();

    // Helper lambda to remove keys matching a pattern
    auto removeKeysMatching = [](auto& data, const std::string& pattern) {
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

  } catch (const Exiv2::Error& e) {
    throw std::runtime_error("Exiv2 error while clearing: " + std::string(e.what()));
  }
}

std::string ImageMetadata::to_string() const {
  std::ostringstream oss;

  oss << "ImageMetadata(\n";
  oss << "    ImageHeight=" << ImageHeight << ",\n";
  oss << "    ImageWidth=" << ImageWidth << ",\n";

  // Helper lambda for consistent optional formatting
  auto formatOptional = [](const auto& opt, const std::string& name) -> std::string {
    if constexpr (std::is_same_v<std::decay_t<decltype(opt)>, std::optional<std::string>>) {
      return name + "=" + (opt ? ("'" + *opt + "'") : "None");
    } else if constexpr (std::is_same_v<std::decay_t<decltype(opt)>, std::optional<int>>) {
      return name + "=" + (opt ? std::to_string(*opt) : "None");
    } else {
      // For complex objects with to_string()
      return name + "=" + (opt ? opt->to_string() : "None");
    }
  };

  // Helper for vector formatting
  auto formatVector = [](const std::optional<std::vector<std::string>>& vec, const std::string& name) -> std::string {
    if (!vec) {
      return name + "=None";
    }

    std::ostringstream vss;
    vss << name << "=[";
    for (size_t i = 0; i < vec->size(); ++i) {
      vss << "'" << (*vec)[i] << "'";
      if (i < vec->size() - 1) {
        vss << ", ";
      }
    }
    vss << "]";
    return vss.str();
  };

  oss << "    " << formatOptional(Title, "Title") << ",\n";
  oss << "    " << formatOptional(Description, "Description") << ",\n";
  oss << "    " << formatOptional(RegionInfo, "RegionInfo") << ",\n";
  oss << "    " << formatOptional(Orientation, "Orientation") << ",\n";
  oss << "    " << formatVector(LastKeywordXMP, "LastKeywordXMP") << ",\n";
  oss << "    " << formatVector(TagsList, "TagsList") << ",\n";
  oss << "    " << formatVector(CatalogSets, "CatalogSets") << ",\n";
  oss << "    " << formatVector(HierarchicalSubject, "HierarchicalSubject") << ",\n";
  oss << "    " << formatOptional(KeywordInfo, "KeywordInfo") << ",\n";
  oss << "    " << formatOptional(Country, "Country") << ",\n";
  oss << "    " << formatOptional(City, "City") << ",\n";
  oss << "    " << formatOptional(State, "State") << ",\n";
  oss << "    " << formatOptional(Location, "Location") << ",\n";

  // Handle private member
  std::string pathStr = m_originalPath ? ("'" + m_originalPath->string() + "'") : "None";
  oss << "    OriginalPath=" << pathStr << "\n";

  oss << ")";
  return oss.str();
}
