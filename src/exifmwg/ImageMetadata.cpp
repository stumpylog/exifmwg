#include <utility>

#include "Errors.hpp"
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
                             std::optional<ExifOrientation> orientation, std::optional<KeywordInfoModel> keywordInfo,
                             std::optional<std::string> country, std::optional<std::string> city,
                             std::optional<std::string> state, std::optional<std::string> location) :
    ImageHeight(imageHeight), ImageWidth(imageWidth), Title(std::move(title)), Description(std::move(description)),
    RegionInfo(std::move(regionInfo)), Orientation(orientation), KeywordInfo(std::move(keywordInfo)),
    Country(std::move(country)), City(std::move(city)), State(std::move(state)), Location(std::move(location)) {
}

ImageMetadata::ImageMetadata(const fs::path& path) {
  this->m_originalPath = path;
  if (!fs::exists(path) || !fs::is_regular_file(path)) {
    throw FileAccessError("File does not exist: " + path.string());
  }
  try {
    auto image = Exiv2::ImageFactory::open(path.string());
    image->readMetadata();

    auto& exifData = image->exifData();
    auto& xmpData = image->xmpData();
    auto& iptcData = image->iptcData();

    this->ImageHeight = image->pixelHeight();
    this->ImageWidth = image->pixelWidth();

    // Read all metadata using private methods
    readOrientation(exifData);
    readTitleAndDescription(xmpData, iptcData);
    readLocationData(xmpData, iptcData);
    readRegionInfo(xmpData);
    readKeywordInfo(xmpData);

  } catch (const Exiv2::Error& e) {
    throw Exiv2Error("Exiv2 error while reading: " + std::string(e.what()));
  }
}

void ImageMetadata::toFile(const std::optional<fs::path>& newPath) {
  fs::path targetPath;
  if (newPath.has_value()) {
    targetPath = newPath.value();
  } else if (this->m_originalPath.has_value()) {
    targetPath = this->m_originalPath.value();
  } else {
    throw FileAccessError("Unable to determine the target path");
  }

  if (this->m_originalPath.has_value() && (this->m_originalPath.value() != targetPath)) {
    try {
      fs::copy_file(this->m_originalPath.value(), targetPath, fs::copy_options::overwrite_existing);
    } catch (const fs::filesystem_error& e) {
      throw FileAccessError("Failed to copy file to new path: " + std::string(e.what()));
    }
  }

  try {
    auto image = Exiv2::ImageFactory::open(targetPath.string());
    image->readMetadata();

    auto& xmpData = image->xmpData();
    auto& exifData = image->exifData();
    auto& iptcData = image->iptcData();

    // Write all metadata using private methods
    writeTitleAndDescription(xmpData, iptcData);
    writeOrientation(exifData);
    writeLocationData(xmpData, iptcData);
    writeRegionInfo(xmpData);
    writeKeywordInfo(xmpData);

    image->writeMetadata();
  } catch (const Exiv2::Error& e) {
    throw Exiv2Error("Exiv2 error while writing: " + std::string(e.what()));
  }
}

void ImageMetadata::clearFile(const std::optional<fs::path>& path) {

  fs::path targetPath;
  if (path.has_value()) {
    targetPath = path.value();
  } else if (this->m_originalPath.has_value()) {
    targetPath = this->m_originalPath.value();
  } else {
    throw FileAccessError("Unable to determine the target path");
  }

  try {
    auto image = Exiv2::ImageFactory::open(targetPath.string());
    image->readMetadata();

    auto& xmpData = image->xmpData();
    auto& exifData = image->exifData();
    auto& iptcData = image->iptcData();

    // Clear all metadata using private methods
    clearRegionInfo(xmpData);
    clearOrientation(exifData);
    clearKeywordInfo(xmpData);
    clearTitleAndDescription(xmpData, iptcData, exifData);

    image->writeMetadata();

  } catch (const Exiv2::Error& e) {
    throw Exiv2Error("Exiv2 error while clearing: " + std::string(e.what()));
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
    } else if constexpr (std::is_same_v<std::decay_t<decltype(opt)>, std::optional<ExifOrientation>>) {
      return name + "=" + (opt ? orientation_to_string(*opt) : "None");
    } else {
      // For complex objects with to_string()
      return name + "=" + (opt ? opt->to_string() : "None");
    }
  };

  oss << "    " << formatOptional(Title, "Title") << ",\n";
  oss << "    " << formatOptional(Description, "Description") << ",\n";
  oss << "    " << formatOptional(RegionInfo, "RegionInfo") << ",\n";
  oss << "    " << formatOptional(Orientation, "Orientation") << ",\n";
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

// Private helper methods for reading metadata
void ImageMetadata::readOrientation(const Exiv2::ExifData& exifData) {
  auto orientKey = exifData.findKey(Exiv2::ExifKey(MetadataKeys::Exif::Orientation));
  if (orientKey != exifData.end()) {
    this->Orientation = orientation_from_exif_value(static_cast<int>(orientKey->toInt64()));
  } else {
    this->Orientation = std::nullopt;
  }
}

void ImageMetadata::readTitleAndDescription(const Exiv2::XmpData& xmpData, const Exiv2::IptcData& iptcData) {
  // Title
  auto titleKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::Title));
  if (titleKey != xmpData.end()) {
    this->Title = XmpUtils::cleanXmpText(titleKey->toString());
  } else {
    this->Title = std::nullopt;
  }

  // Description - try XMP first, then IPTC fallback
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
}

void ImageMetadata::readLocationData(const Exiv2::XmpData& xmpData, const Exiv2::IptcData& iptcData) {
  // Country - try IPTC first, then XMP fallback
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

  // City - try IPTC first, then XMP fallback
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

  // State - try IPTC first, then XMP fallback
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

  // Location - try IPTC first, then XMP fallback
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
}

void ImageMetadata::readRegionInfo(const Exiv2::XmpData& xmpData) {
  auto regionInfoKey = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::Regions));
  if (regionInfoKey != xmpData.end()) {
    this->RegionInfo = RegionInfoStruct::fromXmp(xmpData);
  } else {
    this->RegionInfo = std::nullopt;
  }
}

void ImageMetadata::readKeywordInfo(const Exiv2::XmpData& xmpData) {
  this->KeywordInfo = KeywordInfoModel::fromXmp(xmpData);
}

// Private helper methods for writing metadata
void ImageMetadata::writeTitleAndDescription(Exiv2::XmpData& xmpData, Exiv2::IptcData& iptcData) {
  if (this->Title) {
    xmpData[MetadataKeys::Xmp::Title] = *this->Title;
  }
  if (this->Description) {
    xmpData[MetadataKeys::Xmp::Description] = *this->Description;
    iptcData[MetadataKeys::Iptc::Caption] = *this->Description;
  }
}

void ImageMetadata::writeOrientation(Exiv2::ExifData& exifData) {
  if (this->Orientation) {
    exifData[MetadataKeys::Exif::Orientation] = orientation_to_exif_value(*this->Orientation);
  }
}

void ImageMetadata::writeLocationData(Exiv2::XmpData& xmpData, Exiv2::IptcData& iptcData) {
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
}

void ImageMetadata::writeRegionInfo(Exiv2::XmpData& xmpData) {
  if (this->RegionInfo) {
    this->RegionInfo.value().toXmp(xmpData);
  }
}

void ImageMetadata::writeKeywordInfo(Exiv2::XmpData& xmpData) {
  if (this->KeywordInfo) {
    this->KeywordInfo.value().toXmp(xmpData);
  }
}

// Private helper methods for clearing metadata
void ImageMetadata::clearRegionInfo(Exiv2::XmpData& xmpData) {
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

  removeKeysMatching(xmpData, "Xmp.mwg-rs.Regions");
}

void ImageMetadata::clearOrientation(Exiv2::ExifData& exifData) {
  auto orientIt = exifData.findKey(Exiv2::ExifKey(MetadataKeys::Exif::Orientation));
  if (orientIt != exifData.end()) {
    exifData.erase(orientIt);
  }
}

void ImageMetadata::clearKeywordInfo(Exiv2::XmpData& xmpData) {
  // Clear all keyword-related metadata keys
  std::vector<std::string> keywordKeys = {
      MetadataKeys::Xmp::Keywords,           MetadataKeys::Xmp::KeywordInfo,
      MetadataKeys::Xmp::AcdseeCategories,   MetadataKeys::Xmp::MicrosoftLastKeywordXMP,
      MetadataKeys::Xmp::DigiKamTagsList,    MetadataKeys::Xmp::LightroomHierarchicalSubject,
      MetadataKeys::Xmp::MediaProCatalogSets};

  for (const auto& keyStr : keywordKeys) {
    auto keyIt = xmpData.findKey(Exiv2::XmpKey(keyStr));
    if (keyIt != xmpData.end()) {
      xmpData.erase(keyIt);
    }
  }
}

void ImageMetadata::clearTitleAndDescription(Exiv2::XmpData& xmpData, Exiv2::IptcData& iptcData,
                                             Exiv2::ExifData& exifData) {
  // Clear XMP title and description
  auto titleIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::Title));
  if (titleIt != xmpData.end()) {
    xmpData.erase(titleIt);
  }

  auto descIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::Description));
  if (descIt != xmpData.end()) {
    xmpData.erase(descIt);
  }

  // Clear IPTC caption
  auto iptcCaptionIt = iptcData.findKey(Exiv2::IptcKey(MetadataKeys::Iptc::Caption));
  if (iptcCaptionIt != iptcData.end()) {
    iptcData.erase(iptcCaptionIt);
  }

  // Clear EXIF description
  auto exifDescIt = exifData.findKey(Exiv2::ExifKey(MetadataKeys::Exif::ImageDescription));
  if (exifDescIt != exifData.end()) {
    exifData.erase(exifDescIt);
  }
}
