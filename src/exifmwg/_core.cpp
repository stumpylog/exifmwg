#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/map.h>
#include <exiv2/exiv2.hpp>
#include <string>
#include <vector>
#include <optional>
#include <map>
#include <sstream>
#include <algorithm>

namespace nb = nanobind;

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
    std::string SourceFile;
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

class ExifToolWrapper {
private:
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

    KeywordStruct build_keyword_tree(const std::vector<std::string>& keywords, char separator) {
        std::map<std::string, KeywordStruct> roots;

        for (const auto& keyword : keywords) {
            auto parts = split_string(keyword, separator);
            if (parts.empty()) continue;

            std::string rootName = parts[0];
            if (roots.find(rootName) == roots.end()) {
                roots[rootName] = KeywordStruct{rootName, std::nullopt, {}};
            }

            KeywordStruct* current = &roots[rootName];
            for (size_t i = 1; i < parts.size(); ++i) {
                bool found = false;
                for (auto& child : current->Children) {
                    if (child.Keyword == parts[i]) {
                        current = &child;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    current->Children.push_back(KeywordStruct{parts[i], std::nullopt, {}});
                    current = &current->Children.back();
                }
            }
        }

        // For simplicity, return first root or empty struct
        if (!roots.empty()) {
            return roots.begin()->second;
        }
        return KeywordStruct{"", std::nullopt, {}};
    }

public:
    ImageMetadata read_metadata(const std::string& filepath) {
        ImageMetadata metadata;
        metadata.SourceFile = filepath;

        try {
            auto image = Exiv2::ImageFactory::open(filepath);
            if (!image.get()) {
                throw std::runtime_error("Cannot open file: " + filepath);
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
                metadata.ImageWidth = widthKey->toLong();
            }

            auto heightKey = exifData.findKey(Exiv2::ExifKey("Exif.Photo.PixelYDimension"));
            if (heightKey == exifData.end()) {
                heightKey = exifData.findKey(Exiv2::ExifKey("Exif.Image.ImageLength"));
            }
            if (heightKey != exifData.end()) {
                metadata.ImageHeight = heightKey->toLong();
            }

            // Orientation
            auto orientKey = exifData.findKey(Exiv2::ExifKey("Exif.Image.Orientation"));
            if (orientKey != exifData.end()) {
                metadata.Orientation = static_cast<int>(orientKey->toLong());
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

            // Location data
            auto countryKey = xmpData.findKey(Exiv2::XmpKey("Xmp.iptc.CountryName"));
            if (countryKey != xmpData.end()) {
                metadata.Country = countryKey->toString();
            }

            auto cityKey = xmpData.findKey(Exiv2::XmpKey("Xmp.photoshop.City"));
            if (cityKey != xmpData.end()) {
                metadata.City = cityKey->toString();
            }

            auto stateKey = xmpData.findKey(Exiv2::XmpKey("Xmp.photoshop.State"));
            if (stateKey != xmpData.end()) {
                metadata.State = stateKey->toString();
            }

            auto locationKey = xmpData.findKey(Exiv2::XmpKey("Xmp.iptc.Location"));
            if (locationKey != xmpData.end()) {
                metadata.Location = locationKey->toString();
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
            auto image = Exiv2::ImageFactory::open(metadata.SourceFile);
            if (!image.get()) {
                throw std::runtime_error("Cannot open file: " + metadata.SourceFile);
            }

            image->readMetadata();
            auto& xmpData = image->xmpData();
            auto& exifData = image->exifData();

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

            // Write location data
            if (metadata.Country) {
                xmpData["Xmp.iptc.CountryName"] = *metadata.Country;
            }
            if (metadata.City) {
                xmpData["Xmp.photoshop.City"] = *metadata.City;
            }
            if (metadata.State) {
                xmpData["Xmp.photoshop.State"] = *metadata.State;
            }
            if (metadata.Location) {
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

            // Similar for other keyword fields...
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

            image->writeMetadata();

        } catch (const Exiv2::Error& e) {
            throw std::runtime_error("Exiv2 error: " + std::string(e.what()));
        }
    }
};

NB_MODULE(exifmwg, m) {
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

    nb::class_<ExifToolWrapper>(m, "ExifToolWrapper")
        .def(nb::init<>())
        .def("read_metadata", &ExifToolWrapper::read_metadata)
        .def("write_metadata", &ExifToolWrapper::write_metadata);
}
