#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/filesystem.h>
#include <exiv2/exiv2.hpp>
#include <string>
#include <vector>
#include <optional>
#include <map>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nb = nanobind;
namespace fs = std::filesystem;

struct XmpAreaStruct {
    double H;
    double W;
    double X;
    double Y;
    std::optional<double> D;
    std::string Unit;
};

struct DimensionsStruct {
    double H, W;
    std::string Unit;
};

struct RegionStruct {
    XmpAreaStruct Area;
    std::string Name;
    std::string Type;
    std::optional<std::string> Description;
};

struct RegionInfoStruct {
    DimensionsStruct AppliedToDimensions;
    std::vector<RegionStruct> RegionList;
};

struct KeywordStruct {
    std::string Keyword;
    std::optional<bool> Applied;
    std::vector<KeywordStruct> Children;
};

struct KeywordInfoModel {
    std::vector<KeywordStruct> Hierarchy;
};

struct ImageMetadata {
    fs::path SourceFile;
    int ImageHeight = -1;
    int ImageWidth = -1;
    std::optional<std::string> Title;
    std::optional<std::string> Description;
    std::optional<RegionInfoStruct> RegionInfo;
    std::optional<int> Orientation;
    std::optional<std::vector<std::string>> LastKeywordXMP;
    std::optional<std::vector<std::string>> TagsList;
    std::optional<std::vector<std::string>> CatalogSets;
    std::optional<std::vector<std::string>> HierarchicalSubject;
    std::optional<KeywordInfoModel> KeywordInfo;
    std::optional<std::string> Country;
    std::optional<std::string> City;
    std::optional<std::string> State;
    std::optional<std::string> Location;
};

// Helper functions
std::vector<std::string> split_string(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

XmpAreaStruct parse_area_struct(const std::string& value) {
    // Parse XMP area struct format: "stArea:h=0.123,stArea:unit=normalized,stArea:w=0.456,stArea:x=0.789,stArea:y=0.012"
    XmpAreaStruct area;
    area.Unit = "normalized"; // default

    auto pairs = split_string(value, ',');
    for (const auto& pair : pairs) {
        auto kv = split_string(pair, '=');
        if (kv.size() != 2) continue;

        std::string key = kv[0];
        std::string val = kv[1];

        if (key.find(":h") != std::string::npos) {
            area.H = std::stod(val);
        } else if (key.find(":w") != std::string::npos) {
            area.W = std::stod(val);
        } else if (key.find(":x") != std::string::npos) {
            area.X = std::stod(val);
        } else if (key.find(":y") != std::string::npos) {
            area.Y = std::stod(val);
        } else if (key.find(":d") != std::string::npos) {
            area.D = std::stod(val);
        } else if (key.find(":unit") != std::string::npos) {
            area.Unit = val;
        }
    }
    return area;
}

DimensionsStruct parse_dimensions_struct(const std::string& value) {
    DimensionsStruct dims;
    dims.Unit = "pixel"; // default

    auto pairs = split_string(value, ',');
    for (const auto& pair : pairs) {
        auto kv = split_string(pair, '=');
        if (kv.size() != 2) continue;

        std::string key = kv[0];
        std::string val = kv[1];

        if (key.find(":h") != std::string::npos) {
            dims.H = std::stod(val);
        } else if (key.find(":w") != std::string::npos) {
            dims.W = std::stod(val);
        } else if (key.find(":unit") != std::string::npos) {
            dims.Unit = val;
        }
    }
    return dims;
}

RegionInfoStruct parse_region_info(const Exiv2::XmpData& xmpData) {
    RegionInfoStruct regionInfo;

    // Parse AppliedToDimensions
    auto dimKey = xmpData.findKey(Exiv2::XmpKey("Xmp.mwg-rs.Regions/mwg-rs:AppliedToDimensions"));
    if (dimKey != xmpData.end()) {
        regionInfo.AppliedToDimensions = parse_dimensions_struct(dimKey->toString());
    }

    // Parse RegionList
    int regionIndex = 1;
    while (true) {
        std::string baseKey = "Xmp.mwg-rs.Regions/mwg-rs:RegionList[" + std::to_string(regionIndex) + "]";

        auto areaKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/mwg-rs:Area"));
        if (areaKey == xmpData.end()) break;

        RegionStruct region;
        region.Area = parse_area_struct(areaKey->toString());

        auto nameKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/mwg-rs:Name"));
        if (nameKey != xmpData.end()) {
            region.Name = nameKey->toString();
        }

        auto typeKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/mwg-rs:Type"));
        if (typeKey != xmpData.end()) {
            region.Type = typeKey->toString();
        }

        auto descKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/mwg-rs:Description"));
        if (descKey != xmpData.end()) {
            region.Description = descKey->toString();
        }

        regionInfo.RegionList.push_back(region);
        regionIndex++;
    }

    return regionInfo;
}

std::vector<std::string> parse_string_array(const Exiv2::XmpData& xmpData, const std::string& keyPrefix) {
    std::vector<std::string> result;
    int index = 1;

    while (true) {
        std::string key = keyPrefix + "[" + std::to_string(index) + "]";
        auto it = xmpData.findKey(Exiv2::XmpKey(key));
        if (it == xmpData.end()) break;
        result.push_back(it->toString());
        index++;
    }

    return result;
}

// Main functions to export
ImageMetadata read_metadata(const fs::path& filepath) {
    ImageMetadata metadata;
    metadata.SourceFile = filepath;

    try {
        auto image = Exiv2::ImageFactory::open(filepath.string());
        if (!image.get()) {
            throw std::runtime_error("Cannot open file: " + filepath.string());
        }

        image->readMetadata();

        auto& exifData = image->exifData();
        auto& xmpData = image->xmpData();
        auto& iptcData = image->iptcData();

        // Basic image dimensions
        auto widthKey = exifData.findKey(Exiv2::ExifKey("Exif.Photo.PixelXDimension"));
        if (widthKey == exifData.end()) {
            widthKey = exifData.findKey(Exiv2::ExifKey("Exif.Image.ImageWidth"));
        }
        if (widthKey != exifData.end()) {
            metadata.ImageWidth = static_cast<int>(widthKey->toInt64());
        }

        auto heightKey = exifData.findKey(Exiv2::ExifKey("Exif.Photo.PixelYDimension"));
        if (heightKey == exifData.end()) {
            heightKey = exifData.findKey(Exiv2::ExifKey("Exif.Image.ImageLength"));
        }
        if (heightKey != exifData.end()) {
            metadata.ImageHeight = static_cast<int>(heightKey->toInt64());
        }

        // Orientation
        auto orientKey = exifData.findKey(Exiv2::ExifKey("Exif.Image.Orientation"));
        if (orientKey != exifData.end()) {
            metadata.Orientation = static_cast<int>(orientKey->toInt64());
        }

        // Title and Description
        auto titleKey = xmpData.findKey(Exiv2::XmpKey("Xmp.dc.title"));
        if (titleKey != xmpData.end()) {
            metadata.Title = titleKey->toString();
        }

        auto descKey = xmpData.findKey(Exiv2::XmpKey("Xmp.dc.description"));
        if (descKey != xmpData.end()) {
            metadata.Description = descKey->toString();
        }

        // Location data - try IPTC first, then XMP fallback
        auto countryKey = iptcData.findKey(Exiv2::IptcKey("Iptc.Application2.CountryName"));
        if (countryKey != iptcData.end()) {
            metadata.Country = countryKey->toString();
        } else {
            auto xmpCountryKey = xmpData.findKey(Exiv2::XmpKey("Xmp.iptc.CountryName"));
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

        auto stateKey = iptcData.findKey(Exiv2::IptcKey("Iptc.Application2.ProvinceState"));
        if (stateKey != iptcData.end()) {
            metadata.State = stateKey->toString();
        } else {
            auto xmpStateKey = xmpData.findKey(Exiv2::XmpKey("Xmp.photoshop.State"));
            if (xmpStateKey != xmpData.end()) {
                metadata.State = xmpStateKey->toString();
            }
        }

        auto locationKey = iptcData.findKey(Exiv2::IptcKey("Iptc.Application2.SubLocation"));
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
            try {
                metadata.RegionInfo = parse_region_info(xmpData);
            } catch (const std::exception& e) {
                // Region parsing failed, continue without it
            }
        }

        // Keywords
        auto lastKeywordXMP = parse_string_array(xmpData, "Xmp.microsoft.LastKeywordXMP");
        if (!lastKeywordXMP.empty()) {
            metadata.LastKeywordXMP = lastKeywordXMP;
        }

        auto tagsList = parse_string_array(xmpData, "Xmp.digiKam.TagsList");
        if (!tagsList.empty()) {
            metadata.TagsList = tagsList;
        }

        auto catalogSets = parse_string_array(xmpData, "Xmp.mediapro.CatalogSets");
        if (!catalogSets.empty()) {
            metadata.CatalogSets = catalogSets;
        }

        auto hierarchicalSubject = parse_string_array(xmpData, "Xmp.lr.hierarchicalSubject");
        if (!hierarchicalSubject.empty()) {
            metadata.HierarchicalSubject = hierarchicalSubject;
        }

    } catch (const Exiv2::Error& e) {
        throw std::runtime_error("Exiv2 error: " + std::string(e.what()));
    }

    return metadata;
}

void write_metadata(const ImageMetadata& metadata) {
    try {
        auto image = Exiv2::ImageFactory::open(metadata.SourceFile.string());
        if (!image.get()) {
            throw std::runtime_error("Cannot open file: " + metadata.SourceFile.string());
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
        if (metadata.City) {
            iptcData["Iptc.Application2.City"] = *metadata.City;
            xmpData["Xmp.photoshop.City"] = *metadata.City;
        }
        if (metadata.State) {
            iptcData["Iptc.Application2.ProvinceState"] = *metadata.State;
            xmpData["Xmp.photoshop.State"] = *metadata.State;
        }
        if (metadata.Location) {
            iptcData["Iptc.Application2.SubLocation"] = *metadata.Location;
            xmpData["Xmp.iptc.Location"] = *metadata.Location;
        }

        // Write keyword arrays
        if (metadata.LastKeywordXMP) {
            // Clear existing entries first
            auto it = xmpData.begin();
            while (it != xmpData.end()) {
                if (it->key().find("Xmp.microsoft.LastKeywordXMP") != std::string::npos) {
                    it = xmpData.erase(it);
                } else {
                    ++it;
                }
            }

            for (size_t i = 0; i < metadata.LastKeywordXMP->size(); ++i) {
                std::string key = "Xmp.microsoft.LastKeywordXMP[" + std::to_string(i + 1) + "]";
                xmpData[key] = (*metadata.LastKeywordXMP)[i];
            }
        }

        if (metadata.TagsList) {
            auto it = xmpData.begin();
            while (it != xmpData.end()) {
                if (it->key().find("Xmp.digiKam.TagsList") != std::string::npos) {
                    it = xmpData.erase(it);
                } else {
                    ++it;
                }
            }

            for (size_t i = 0; i < metadata.TagsList->size(); ++i) {
                std::string key = "Xmp.digiKam.TagsList[" + std::to_string(i + 1) + "]";
                xmpData[key] = (*metadata.TagsList)[i];
            }
        }

        if (metadata.CatalogSets) {
            auto it = xmpData.begin();
            while (it != xmpData.end()) {
                if (it->key().find("Xmp.mediapro.CatalogSets") != std::string::npos) {
                    it = xmpData.erase(it);
                } else {
                    ++it;
                }
            }

            for (size_t i = 0; i < metadata.CatalogSets->size(); ++i) {
                std::string key = "Xmp.mediapro.CatalogSets[" + std::to_string(i + 1) + "]";
                xmpData[key] = (*metadata.CatalogSets)[i];
            }
        }

        if (metadata.HierarchicalSubject) {
            auto it = xmpData.begin();
            while (it != xmpData.end()) {
                if (it->key().find("Xmp.lr.hierarchicalSubject") != std::string::npos) {
                    it = xmpData.erase(it);
                } else {
                    ++it;
                }
            }

            for (size_t i = 0; i < metadata.HierarchicalSubject->size(); ++i) {
                std::string key = "Xmp.lr.hierarchicalSubject[" + std::to_string(i + 1) + "]";
                xmpData[key] = (*metadata.HierarchicalSubject)[i];
            }
        }

        image->writeMetadata();

    } catch (const Exiv2::Error& e) {
        throw std::runtime_error("Exiv2 error: " + std::string(e.what()));
    }
}

NB_MODULE(exifmwg, m) {
    // Only export the ImageMetadata struct and the two main functions
    nb::class_<ImageMetadata>(m, "ImageMetadata")
        .def(nb::init<>())
        .def_rw("SourceFile", &ImageMetadata::SourceFile)
        .def_rw("ImageHeight", &ImageMetadata::ImageHeight)
        .def_rw("ImageWidth", &ImageMetadata::ImageWidth)
        .def_rw("Title", &ImageMetadata::Title)
        .def_rw("Description", &ImageMetadata::Description)
        .def_rw("RegionInfo", &ImageMetadata::RegionInfo)
        .def_rw("Orientation", &ImageMetadata::Orientation)
        .def_rw("LastKeywordXMP", &ImageMetadata::LastKeywordXMP)
        .def_rw("TagsList", &ImageMetadata::TagsList)
        .def_rw("CatalogSets", &ImageMetadata::CatalogSets)
        .def_rw("HierarchicalSubject", &ImageMetadata::HierarchicalSubject)
        .def_rw("KeywordInfo", &ImageMetadata::KeywordInfo)
        .def_rw("Country", &ImageMetadata::Country)
        .def_rw("City", &ImageMetadata::City)
        .def_rw("State", &ImageMetadata::State)
        .def_rw("Location", &ImageMetadata::Location);

    // Export supporting structs that are part of ImageMetadata
    nb::class_<XmpAreaStruct>(m, "XmpAreaStruct")
        .def(nb::init<>())
        .def_rw("H", &XmpAreaStruct::H)
        .def_rw("W", &XmpAreaStruct::W)
        .def_rw("X", &XmpAreaStruct::X)
        .def_rw("Y", &XmpAreaStruct::Y)
        .def_rw("D", &XmpAreaStruct::D)
        .def_rw("Unit", &XmpAreaStruct::Unit);

    nb::class_<DimensionsStruct>(m, "DimensionsStruct")
        .def(nb::init<>())
        .def_rw("H", &DimensionsStruct::H)
        .def_rw("W", &DimensionsStruct::W)
        .def_rw("Unit", &DimensionsStruct::Unit);

    nb::class_<RegionStruct>(m, "RegionStruct")
        .def(nb::init<>())
        .def_rw("Area", &RegionStruct::Area)
        .def_rw("Name", &RegionStruct::Name)
        .def_rw("Type", &RegionStruct::Type)
        .def_rw("Description", &RegionStruct::Description);

    nb::class_<RegionInfoStruct>(m, "RegionInfoStruct")
        .def(nb::init<>())
        .def_rw("AppliedToDimensions", &RegionInfoStruct::AppliedToDimensions)
        .def_rw("RegionList", &RegionInfoStruct::RegionList);

    nb::class_<KeywordStruct>(m, "KeywordStruct")
        .def(nb::init<>())
        .def_rw("Keyword", &KeywordStruct::Keyword)
        .def_rw("Applied", &KeywordStruct::Applied)
        .def_rw("Children", &KeywordStruct::Children);

    nb::class_<KeywordInfoModel>(m, "KeywordInfoModel")
        .def(nb::init<>())
        .def_rw("Hierarchy", &KeywordInfoModel::Hierarchy);

    // Export the main functions
    m.def("read_metadata", &read_metadata, "Read metadata from an image file");
    m.def("write_metadata", &write_metadata, "Write metadata to an image file");
}
