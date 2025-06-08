#include <algorithm>
#include <exiv2/exiv2.hpp>
#include <filesystem>
#include <iostream>
#include <map>
#include <nanobind/nanobind.h>
#include <nanobind/stl/filesystem.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
namespace nb = nanobind;
namespace fs = std::filesystem;

using namespace nb::literals;

struct XmpAreaStruct
{
    double                H;
    double                W;
    double                X;
    double                Y;
    std::string           Unit;
    std::optional<double> D;

    XmpAreaStruct(double h, double w, double x, double y,
        const std::string& unit, std::optional<double> d)
        : H(h), W(w), X(x), Y(y), Unit(unit), D(d)
    {
    }
};

struct DimensionsStruct
{
    double      H;
    double      W;
    std::string Unit;

    DimensionsStruct(double h, double w, const std::string& unit)
        : H(h), W(w), Unit(unit)
    {
    }
};

struct RegionStruct
{
    XmpAreaStruct              Area;
    std::string                Name;
    std::string                Type;
    std::optional<std::string> Description;

    RegionStruct(const XmpAreaStruct& area, const std::string& name,
        const std::string& type, std::optional<std::string> description)
        : Area(area), Name(name), Type(type), Description(description)
    {
    }
};

struct RegionInfoStruct
{
    DimensionsStruct          AppliedToDimensions;
    std::vector<RegionStruct> RegionList;

    RegionInfoStruct(const DimensionsStruct& appliedToDimensions,
        const std::vector<RegionStruct>&     regionList)
        : AppliedToDimensions(appliedToDimensions), RegionList(regionList)
    {
    }
};

struct KeywordStruct
{
    std::string                Keyword;
    std::optional<bool>        Applied;
    std::vector<KeywordStruct> Children;

    KeywordStruct(const std::string&      keyword,
        const std::vector<KeywordStruct>& children, std::optional<bool> applied)
        : Keyword(keyword), Applied(applied), Children(children)
    {
    }
};

struct KeywordInfoModel
{
    std::vector<KeywordStruct> Hierarchy;

    KeywordInfoModel(const std::vector<KeywordStruct>& hierarchy)
        : Hierarchy(hierarchy)
    {
    }
};

struct ImageMetadata
{
    fs::path                                SourceFile;
    int                                     ImageHeight;
    int                                     ImageWidth;
    std::optional<std::string>              Title;
    std::optional<std::string>              Description;
    std::optional<RegionInfoStruct>         RegionInfo;
    std::optional<int>                      Orientation;
    std::optional<std::vector<std::string>> LastKeywordXMP;
    std::optional<std::vector<std::string>> TagsList;
    std::optional<std::vector<std::string>> CatalogSets;
    std::optional<std::vector<std::string>> HierarchicalSubject;
    std::optional<KeywordInfoModel>         KeywordInfo;
    std::optional<std::string>              Country;
    std::optional<std::string>              City;
    std::optional<std::string>              State;
    std::optional<std::string>              Location;

    ImageMetadata(const fs::path& sourceFile, int imageHeight, int imageWidth,
        std::optional<std::string>              title,
        std::optional<std::string>              description,
        std::optional<RegionInfoStruct>         regionInfo,
        std::optional<int>                      orientation,
        std::optional<std::vector<std::string>> lastKeywordXMP,
        std::optional<std::vector<std::string>> tagsList,
        std::optional<std::vector<std::string>> catalogSets,
        std::optional<std::vector<std::string>> hierarchicalSubject,
        std::optional<KeywordInfoModel>         keywordInfo,
        std::optional<std::string> country, std::optional<std::string> city,
        std::optional<std::string> state, std::optional<std::string> location)
        : SourceFile(sourceFile), ImageHeight(imageHeight),
          ImageWidth(imageWidth), Title(title), Description(description),
          RegionInfo(regionInfo), Orientation(orientation),
          LastKeywordXMP(lastKeywordXMP), TagsList(tagsList),
          CatalogSets(catalogSets), HierarchicalSubject(hierarchicalSubject),
          KeywordInfo(keywordInfo), Country(country), City(city), State(state),
          Location(location)
    {
    }
};

// XmpAreaStruct equality
bool operator==(const XmpAreaStruct& lhs, const XmpAreaStruct& rhs)
{
    return lhs.H == rhs.H && lhs.W == rhs.W && lhs.X == rhs.X &&
           lhs.Y == rhs.Y && lhs.Unit == rhs.Unit && lhs.D == rhs.D;
}

// DimensionsStruct equality
bool operator==(const DimensionsStruct& lhs, const DimensionsStruct& rhs)
{
    return lhs.H == rhs.H && lhs.W == rhs.W && lhs.Unit == rhs.Unit;
}

// RegionStruct equality
bool operator==(const RegionStruct& lhs, const RegionStruct& rhs)
{
    return lhs.Area == rhs.Area && lhs.Name == rhs.Name &&
           lhs.Type == rhs.Type && lhs.Description == rhs.Description;
}

// RegionInfoStruct equality
bool operator==(const RegionInfoStruct& lhs, const RegionInfoStruct& rhs)
{
    return lhs.AppliedToDimensions == rhs.AppliedToDimensions &&
           lhs.RegionList == rhs.RegionList;
}

// KeywordStruct equality
bool operator==(const KeywordStruct& lhs, const KeywordStruct& rhs)
{
    return lhs.Keyword == rhs.Keyword && lhs.Applied == rhs.Applied &&
           lhs.Children == rhs.Children;
}

// KeywordInfoModel equality
bool operator==(const KeywordInfoModel& lhs, const KeywordInfoModel& rhs)
{
    return lhs.Hierarchy == rhs.Hierarchy;
}

// ImageMetadata equality
bool operator==(const ImageMetadata& lhs, const ImageMetadata& rhs)
{
    return lhs.SourceFile == rhs.SourceFile &&
           lhs.ImageHeight == rhs.ImageHeight &&
           lhs.ImageWidth == rhs.ImageWidth && lhs.Title == rhs.Title &&
           lhs.Description == rhs.Description &&
           lhs.RegionInfo == rhs.RegionInfo &&
           lhs.Orientation == rhs.Orientation &&
           lhs.LastKeywordXMP == rhs.LastKeywordXMP &&
           lhs.TagsList == rhs.TagsList && lhs.CatalogSets == rhs.CatalogSets &&
           lhs.HierarchicalSubject == rhs.HierarchicalSubject &&
           lhs.KeywordInfo == rhs.KeywordInfo && lhs.Country == rhs.Country &&
           lhs.City == rhs.City && lhs.State == rhs.State &&
           lhs.Location == rhs.Location;
}

void log_to_python(const std::string& level, const std::string& message)
{
    try
    {
        // Get Python's logging module
        nb::module_ logging = nb::module_::import_("logging");

        // Get the logger for this module
        nb::object logger = logging.attr("getLogger")("exifmwg");

        // Call the appropriate logging method
        if (level == "debug")
        {
            logger.attr("debug")(message);
        }
        else if (level == "info")
        {
            logger.attr("info")(message);
        }
        else if (level == "warning")
        {
            logger.attr("warning")(message);
        }
        else if (level == "error")
        {
            logger.attr("error")(message);
        }
        else
        {
            logger.attr("info")(message);
        }
    }
    catch (const std::exception& e)
    {
    }
    std::cerr << message << std::endl;
}

#define LOG_DEBUG(msg) log_to_python("debug", msg)
#define LOG_INFO(msg) log_to_python("info", msg)
#define LOG_WARNING(msg) log_to_python("warning", msg)
#define LOG_ERROR(msg) log_to_python("error", msg)

std::vector<std::string> split_string(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream        ss(str);
    std::string              token;
    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string clean_xmp_text(const std::string& xmpValue)
{
    // Handle XMP localized text format: lang="x-default" Actual text content
    std::string cleaned = xmpValue;

    // Look for the pattern lang="x-default" or similar language tags
    size_t langPos = cleaned.find("lang=\"");
    if (langPos != std::string::npos)
    {
        // Find the end of the language attribute (closing quote + space)
        size_t quoteEnd =
            cleaned.find("\"", langPos + 6); // 6 = length of "lang=\""
        if (quoteEnd != std::string::npos)
        {
            // Skip past the quote and any following whitespace
            size_t textStart = quoteEnd + 1;
            while (textStart < cleaned.length() &&
                   std::isspace(cleaned[textStart]))
            {
                textStart++;
            }
            cleaned = cleaned.substr(textStart);
        }
    }

    return cleaned;
}

XmpAreaStruct parse_area_struct(
    const Exiv2::XmpData& xmpData, const std::string& baseKey)
{
    double                h = 0.0, w = 0.0, x = 0.0, y = 0.0;
    std::optional<double> d;
    std::string           unit = "normalized";

    auto hKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:h"));
    if (hKey != xmpData.end())
    {
        h = std::stod(hKey->toString());
    }

    auto wKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:w"));
    if (wKey != xmpData.end())
    {
        w = std::stod(wKey->toString());
    }

    auto xKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:x"));
    if (xKey != xmpData.end())
    {
        x = std::stod(xKey->toString());
    }

    auto yKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:y"));
    if (yKey != xmpData.end())
    {
        y = std::stod(yKey->toString());
    }

    auto dKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:d"));
    if (dKey != xmpData.end())
    {
        d = std::stod(dKey->toString());
    }

    auto unitKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stArea:unit"));
    if (unitKey != xmpData.end())
    {
        unit = unitKey->toString();
    }

    return XmpAreaStruct(h, w, x, y, unit, d);
}

DimensionsStruct parse_dimensions_struct(
    const Exiv2::XmpData& xmpData, const std::string& baseKey)
{
    double      h = 0.0, w = 0.0;
    std::string unit = "pixel";

    // Parse individual dimension fields
    auto hKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stDim:h"));
    if (hKey != xmpData.end())
    {
        try
        {
            h = std::stod(hKey->toString());
        }
        catch (const std::exception&)
        {
            // Handle parsing error
        }
    }

    auto wKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stDim:w"));
    if (wKey != xmpData.end())
    {
        try
        {
            w = std::stod(wKey->toString());
        }
        catch (const std::exception&)
        {
            // Handle parsing error
        }
    }

    auto unitKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/stDim:unit"));
    if (unitKey != xmpData.end())
    {
        unit = unitKey->toString();
    }

    if (h == 0.0)
    {
        throw std::runtime_error("No height found in dimensions struct");
    }

    return DimensionsStruct(h, w, unit);
}

RegionInfoStruct parse_region_info(const Exiv2::XmpData& xmpData)
{
    DimensionsStruct          appliedToDimensions_val(0.0, 0.0, "pixel");
    std::vector<RegionStruct> regionList_val;

    LOG_DEBUG("Parsing RegionInfo");

    // Parse AppliedToDimensions
    try
    {
        appliedToDimensions_val = parse_dimensions_struct(
            xmpData, "Xmp.mwg-rs.Regions/mwg-rs:AppliedToDimensions");
    }
    catch (const std::exception&)
    {
        throw std::runtime_error("Expected AppliedToDimensions to exist");
    }

    LOG_DEBUG("Found AppliedToDimensions");

    // Parse RegionList
    int regionIndex = 1;
    while (true)
    {
        std::string baseKey = "Xmp.mwg-rs.Regions/mwg-rs:RegionList[" +
                              std::to_string(regionIndex) + "]";

        // Check if this region exists
        auto regionKey = xmpData.findKey(Exiv2::XmpKey(baseKey));
        if (regionKey == xmpData.end())
            break;

        XmpAreaStruct area =
            parse_area_struct(xmpData, baseKey + "/mwg-rs:Area");

        std::string                name_val = "";
        std::string                type_val = "";
        std::optional<std::string> desc_val;

        auto nameKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/mwg-rs:Name"));
        if (nameKey != xmpData.end())
        {
            name_val = clean_xmp_text(nameKey->toString());
        }

        auto typeKey = xmpData.findKey(Exiv2::XmpKey(baseKey + "/mwg-rs:Type"));
        if (typeKey != xmpData.end())
        {
            type_val = typeKey->toString();
        }

        auto descKey =
            xmpData.findKey(Exiv2::XmpKey(baseKey + "/mwg-rs:Description"));
        if (descKey != xmpData.end())
        {
            desc_val = descKey->toString();
        }

        regionList_val.push_back(
            RegionStruct(area, name_val, type_val, desc_val));
        regionIndex++;
    }

    return RegionInfoStruct(appliedToDimensions_val, regionList_val);
}

std::vector<std::string> parse_delimited_string(
    const Exiv2::XmpData& xmpData, const std::string& key, char delimiter = ',')
{
    std::vector<std::string> result;

    auto it = xmpData.findKey(Exiv2::XmpKey(key));
    if (it != xmpData.end())
    {
        std::string value = it->toString();

        // Split on delimiter
        std::vector<std::string> tokens = split_string(value, delimiter);
        for (const auto& token : tokens)
        {
            std::string trimmed = trim(token);
            if (!trimmed.empty())
            {
                result.push_back(trimmed);
            }
        }
    }

    return result;
}

KeywordStruct parse_keyword_struct(
    const Exiv2::XmpData& xmpData, const std::string& basePath)
{

    // Get the keyword value
    std::string keywordKey = basePath + "/mwg-kw:Keyword";
    auto        keywordIt  = xmpData.findKey(Exiv2::XmpKey(keywordKey));
    std::string keyword    = "";
    if (keywordIt != xmpData.end())
    {
        keyword = keywordIt->toString();
    }

    // Check for Applied attribute (optional)
    std::string         appliedKey = basePath + "/mwg-kw:Applied";
    auto                appliedIt  = xmpData.findKey(Exiv2::XmpKey(appliedKey));
    std::optional<bool> applied;
    if (appliedIt != xmpData.end())
    {
        std::string appliedValue = appliedIt->toString();
        applied = (appliedValue == "True" || appliedValue == "true" ||
                   appliedValue == "1");
    }

    // Parse children recursively
    std::vector<KeywordStruct> children;
    std::string                childrenBasePath = basePath + "/mwg-kw:Children";
    int                        childIndex       = 1;

    while (true)
    {
        std::string childPath =
            childrenBasePath + "[" + std::to_string(childIndex) + "]";
        std::string childKeywordKey = childPath + "/mwg-kw:Keyword";

        // Check if this child exists
        auto childKeywordIt = xmpData.findKey(Exiv2::XmpKey(childKeywordKey));
        if (childKeywordIt == xmpData.end())
        {
            break;
        }

        KeywordStruct child = parse_keyword_struct(xmpData, childPath);
        children.push_back(child);
        childIndex++;
    }

    return KeywordStruct(keyword, children, applied);
}

KeywordInfoModel parse_keyword_info(const Exiv2::XmpData& xmpData)
{
    std::vector<KeywordStruct> hierarchy;
    std::string basePath = "Xmp.mwg-kw.Keywords/mwg-kw:Hierarchy";
    int         index    = 1;

    while (true)
    {
        std::string itemPath   = basePath + "[" + std::to_string(index) + "]";
        std::string keywordKey = itemPath + "/mwg-kw:Keyword";

        // Check if this hierarchy item exists
        auto keywordIt = xmpData.findKey(Exiv2::XmpKey(keywordKey));
        if (keywordIt == xmpData.end())
        {
            break;
        }

        KeywordStruct keywordStruct = parse_keyword_struct(xmpData, itemPath);
        hierarchy.push_back(keywordStruct);
        index++;
    }
    return KeywordInfoModel(hierarchy);
}

void write_keyword_struct(Exiv2::XmpData& xmpData,
    const KeywordStruct& keywordStruct, const std::string& basePath)
{

    // Write the keyword
    std::string keywordKey = basePath + "/mwg-kw:Keyword";
    xmpData[keywordKey]    = keywordStruct.Keyword;

    // Write Applied attribute if present
    if (keywordStruct.Applied)
    {
        std::string appliedKey = basePath + "/mwg-kw:Applied";
        xmpData[appliedKey]    = *keywordStruct.Applied ? "True" : "False";
    }

    // Write children recursively
    if (!keywordStruct.Children.empty())
    {
        for (size_t i = 0; i < keywordStruct.Children.size(); ++i)
        {
            std::string childPath =
                basePath + "/mwg-kw:Children[" + std::to_string(i + 1) + "]";
            write_keyword_struct(xmpData, keywordStruct.Children[i], childPath);
        }
    }
}

void write_keyword_info(
    Exiv2::XmpData& xmpData, const KeywordInfoModel& keywordInfo)
{
    LOG_DEBUG("Writing MWG Keywords hierarchy");

    // Clear existing MWG Keywords data
    auto it = xmpData.begin();
    while (it != xmpData.end())
    {
        if (it->key().find("Xmp.mwg-kw.Keywords") != std::string::npos)
        {
            it = xmpData.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Write hierarchy
    std::string basePath = "Xmp.mwg-kw.Keywords/mwg-kw:Hierarchy";
    for (size_t i = 0; i < keywordInfo.Hierarchy.size(); ++i)
    {
        std::string itemPath = basePath + "[" + std::to_string(i + 1) + "]";
        write_keyword_struct(xmpData, keywordInfo.Hierarchy[i], itemPath);
    }

    LOG_DEBUG("Wrote " + std::to_string(keywordInfo.Hierarchy.size()) +
              " top-level keyword hierarchy items");
}

// Main functions to export
ImageMetadata read_metadata(const fs::path& filepath)
{
    // Initialize with dummy/default values, then fill.
    // This is necessary because ImageMetadata no longer has a default
    // constructor.
    ImageMetadata metadata(
        filepath, -1, -1, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {});

    try
    {
        auto image = Exiv2::ImageFactory::open(filepath.string());
        if (!image.get())
        {
            throw std::runtime_error("Cannot open file: " + filepath.string());
        }

        image->readMetadata();

        auto& exifData = image->exifData();
        auto& xmpData  = image->xmpData();
        auto& iptcData = image->iptcData();

        // Basic image dimensions
        // TODO: Consider "Exif.Photo.PixelXDimension" and
        // "Exif.Image.ImageWidth" (and the equivalents Y) for something
        metadata.ImageWidth  = image->pixelWidth();
        metadata.ImageHeight = image->pixelHeight();

        // Orientation
        auto orientKey =
            exifData.findKey(Exiv2::ExifKey("Exif.Image.Orientation"));
        if (orientKey != exifData.end())
        {
            metadata.Orientation = static_cast<int>(orientKey->toInt64());
        }

        // Title and Description
        auto titleKey = xmpData.findKey(Exiv2::XmpKey("Xmp.dc.title"));
        if (titleKey != xmpData.end())
        {
            metadata.Title = clean_xmp_text(titleKey->toString());
        }

        auto descKey = xmpData.findKey(Exiv2::XmpKey("Xmp.dc.description"));
        if (descKey != xmpData.end())
        {
            metadata.Description = clean_xmp_text(descKey->toString());
        }

        // Location data - try IPTC first, then XMP fallback
        auto countryKey =
            iptcData.findKey(Exiv2::IptcKey("Iptc.Application2.CountryName"));
        if (countryKey != iptcData.end())
        {
            metadata.Country = countryKey->toString();
        }
        else
        {
            auto xmpCountryKey =
                xmpData.findKey(Exiv2::XmpKey("Xmp.iptc.CountryName"));
            if (xmpCountryKey != xmpData.end())
            {
                metadata.Country = xmpCountryKey->toString();
            }
        }

        auto cityKey =
            iptcData.findKey(Exiv2::IptcKey("Iptc.Application2.City"));
        if (cityKey != iptcData.end())
        {
            metadata.City = cityKey->toString();
        }
        else
        {
            auto xmpCityKey =
                xmpData.findKey(Exiv2::XmpKey("Xmp.photoshop.City"));
            if (xmpCityKey != xmpData.end())
            {
                metadata.City = xmpCityKey->toString();
            }
        }

        auto stateKey =
            iptcData.findKey(Exiv2::IptcKey("Iptc.Application2.ProvinceState"));
        if (stateKey != iptcData.end())
        {
            metadata.State = stateKey->toString();
        }
        else
        {
            auto xmpStateKey =
                xmpData.findKey(Exiv2::XmpKey("Xmp.photoshop.State"));
            if (xmpStateKey != xmpData.end())
            {
                metadata.State = xmpStateKey->toString();
            }
        }

        auto locationKey =
            iptcData.findKey(Exiv2::IptcKey("Iptc.Application2.SubLocation"));
        if (locationKey != iptcData.end())
        {
            metadata.Location = locationKey->toString();
        }
        else
        {
            auto xmpLocationKey =
                xmpData.findKey(Exiv2::XmpKey("Xmp.iptc.Location"));
            if (xmpLocationKey != xmpData.end())
            {
                metadata.Location = xmpLocationKey->toString();
            }
        }

        // Region Info
        if (!xmpData.empty())
        {
            try
            {
                metadata.RegionInfo = parse_region_info(xmpData);
            }
            catch (const std::exception& e)
            {
                // Region parsing failed, continue without it
            }
        }

        // Keywords
        auto lastKeywordXMP = parse_delimited_string(
            xmpData, "Xmp.MicrosoftPhoto.LastKeywordXMP", ',');
        if (!lastKeywordXMP.empty())
        {
            metadata.LastKeywordXMP = lastKeywordXMP;
        }

        auto tagsList =
            parse_delimited_string(xmpData, "Xmp.digiKam.TagsList", ',');
        if (!tagsList.empty())
        {
            metadata.TagsList = tagsList;
        }

        auto catalogSets =
            parse_delimited_string(xmpData, "Xmp.mediapro.CatalogSets", ',');
        if (!catalogSets.empty())
        {
            metadata.CatalogSets = catalogSets;
        }

        auto hierarchicalSubject =
            parse_delimited_string(xmpData, "Xmp.lr.hierarchicalSubject", ',');
        if (!hierarchicalSubject.empty())
        {
            metadata.HierarchicalSubject = hierarchicalSubject;
        }
        metadata.KeywordInfo = parse_keyword_info(xmpData);
    }
    catch (const Exiv2::Error& e)
    {
        throw std::runtime_error("Exiv2 error: " + std::string(e.what()));
    }

    return metadata;
}

void write_metadata(const ImageMetadata& metadata)
{
    try
    {
        auto image = Exiv2::ImageFactory::open(metadata.SourceFile.string());
        if (!image.get())
        {
            throw std::runtime_error(
                "Cannot open file: " + metadata.SourceFile.string());
        }

        image->readMetadata();
        auto& xmpData  = image->xmpData();
        auto& exifData = image->exifData();
        auto& iptcData = image->iptcData();

        // Write basic fields
        if (metadata.Title)
        {
            xmpData["Xmp.dc.title"] = *metadata.Title;
        }

        if (metadata.Description)
        {
            xmpData["Xmp.dc.description"] = *metadata.Description;
        }

        if (metadata.Orientation)
        {
            exifData["Exif.Image.Orientation"] = *metadata.Orientation;
        }

        // Write location data to both IPTC and XMP for maximum compatibility
        if (metadata.Country)
        {
            iptcData["Iptc.Application2.CountryName"] = *metadata.Country;
            xmpData["Xmp.iptc.CountryName"]           = *metadata.Country;
        }
        if (metadata.City)
        {
            iptcData["Iptc.Application2.City"] = *metadata.City;
            xmpData["Xmp.photoshop.City"]      = *metadata.City;
        }
        if (metadata.State)
        {
            iptcData["Iptc.Application2.ProvinceState"] = *metadata.State;
            xmpData["Xmp.photoshop.State"]              = *metadata.State;
        }
        if (metadata.Location)
        {
            iptcData["Iptc.Application2.SubLocation"] = *metadata.Location;
            xmpData["Xmp.iptc.Location"]              = *metadata.Location;
        }

        // Write keyword arrays
        if (metadata.LastKeywordXMP)
        {
            // Clear existing entries first
            auto it = xmpData.begin();
            while (it != xmpData.end())
            {
                if (it->key().find("Xmp.MicrosoftPhoto.LastKeywordXMP") !=
                    std::string::npos)
                {
                    it = xmpData.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            for (size_t i = 0; i < metadata.LastKeywordXMP->size(); ++i)
            {
                std::string key = "Xmp.microsoft.LastKeywordXMP[" +
                                  std::to_string(i + 1) + "]";
                xmpData[key] = (*metadata.LastKeywordXMP)[i];
            }
        }

        if (metadata.TagsList)
        {
            auto it = xmpData.begin();
            while (it != xmpData.end())
            {
                if (it->key().find("Xmp.digiKam.TagsList") != std::string::npos)
                {
                    it = xmpData.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            for (size_t i = 0; i < metadata.TagsList->size(); ++i)
            {
                std::string key =
                    "Xmp.digiKam.TagsList[" + std::to_string(i + 1) + "]";
                xmpData[key] = (*metadata.TagsList)[i];
            }
        }

        if (metadata.CatalogSets)
        {
            auto it = xmpData.begin();
            while (it != xmpData.end())
            {
                if (it->key().find("Xmp.mediapro.CatalogSets") !=
                    std::string::npos)
                {
                    it = xmpData.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            for (size_t i = 0; i < metadata.CatalogSets->size(); ++i)
            {
                std::string key =
                    "Xmp.mediapro.CatalogSets[" + std::to_string(i + 1) + "]";
                xmpData[key] = (*metadata.CatalogSets)[i];
            }
        }

        if (metadata.HierarchicalSubject)
        {
            auto it = xmpData.begin();
            while (it != xmpData.end())
            {
                if (it->key().find("Xmp.lr.hierarchicalSubject") !=
                    std::string::npos)
                {
                    it = xmpData.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            for (size_t i = 0; i < metadata.HierarchicalSubject->size(); ++i)
            {
                std::string key =
                    "Xmp.lr.hierarchicalSubject[" + std::to_string(i + 1) + "]";
                xmpData[key] = (*metadata.HierarchicalSubject)[i];
            }
        }

        image->writeMetadata();
    }
    catch (const Exiv2::Error& e)
    {
        throw std::runtime_error("Exiv2 error: " + std::string(e.what()));
    }
}

void clear_existing_metadata(const fs::path& filepath)
{
    try
    {
        auto image = Exiv2::ImageFactory::open(filepath.string());
        if (!image.get())
        {
            throw std::runtime_error("Cannot open file: " + filepath.string());
        }

        image->readMetadata();
        auto& xmpData  = image->xmpData();
        auto& exifData = image->exifData();
        auto& iptcData = image->iptcData();

        // Helper lambda to remove keys matching a pattern
        auto removeKeysMatching = [](auto& data, const std::string& pattern)
        {
            auto it = data.begin();
            while (it != data.end())
            {
                if (it->key().find(pattern) != std::string::npos)
                {
                    it = data.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        };

        // Clear face regions
        removeKeysMatching(xmpData, "Xmp.mwg-rs.Regions");

        // Clear orientation
        auto orientIt =
            exifData.findKey(Exiv2::ExifKey("Exif.Image.Orientation"));
        if (orientIt != exifData.end())
        {
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
        if (titleIt != xmpData.end())
        {
            xmpData.erase(titleIt);
        }

        auto descIt = xmpData.findKey(Exiv2::XmpKey("Xmp.dc.description"));
        if (descIt != xmpData.end())
        {
            xmpData.erase(descIt);
        }

        // EXIF
        auto exifDescIt =
            exifData.findKey(Exiv2::ExifKey("Exif.Image.ImageDescription"));
        if (exifDescIt != exifData.end())
        {
            exifData.erase(exifDescIt);
        }

        auto ifd0DescIt =
            exifData.findKey(Exiv2::ExifKey("Exif.Image.ImageDescription"));
        if (ifd0DescIt != exifData.end())
        {
            exifData.erase(ifd0DescIt);
        }

        // IPTC
        auto iptcCaptionIt =
            iptcData.findKey(Exiv2::IptcKey("Iptc.Application2.Caption"));
        if (iptcCaptionIt != iptcData.end())
        {
            iptcData.erase(iptcCaptionIt);
        }

        image->writeMetadata();
    }
    catch (const Exiv2::Error& e)
    {
        throw std::runtime_error("Exiv2 error: " + std::string(e.what()));
    }
}

NB_MODULE(exifmwg, m)
{
    nb::class_<ImageMetadata>(m, "ImageMetadata")
        .def(nb::init<fs::path, int, int, std::optional<std::string>,
                 std::optional<std::string>, std::optional<RegionInfoStruct>,
                 std::optional<int>, std::optional<std::vector<std::string>>,
                 std::optional<std::vector<std::string>>,
                 std::optional<std::vector<std::string>>,
                 std::optional<std::vector<std::string>>,
                 std::optional<KeywordInfoModel>, std::optional<std::string>,
                 std::optional<std::string>, std::optional<std::string>,
                 std::optional<std::string>>(),

            "SourceFile"_a, "ImageHeight"_a, "ImageWidth"_a,
            "Title"_a = nb::none(), "Description"_a = nb::none(),
            "RegionInfo"_a = nb::none(), "Orientation"_a = nb::none(),
            "LastKeywordXMP"_a = nb::none(), "TagsList"_a = nb::none(),
            "CatalogSets"_a = nb::none(), "HierarchicalSubject"_a = nb::none(),
            "KeywordInfo"_a = nb::none(), "Country"_a = nb::none(),
            "City"_a = nb::none(), "State"_a = nb::none(),
            "Location"_a = nb::none())
        .def("__eq__", [](const ImageMetadata& self, const ImageMetadata& other)
            { return self == other; })
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

    nb::class_<XmpAreaStruct>(m, "XmpAreaStruct")
        .def(nb::init<double, double, double, double, const std::string&,
                 std::optional<double>>(),

            "H"_a, "W"_a, "X"_a, "Y"_a, "Unit"_a, "D"_a = nb::none())
        .def("__eq__", [](const XmpAreaStruct& self, const XmpAreaStruct& other)
            { return self == other; })
        .def_rw("H", &XmpAreaStruct::H)
        .def_rw("W", &XmpAreaStruct::W)
        .def_rw("X", &XmpAreaStruct::X)
        .def_rw("Y", &XmpAreaStruct::Y)
        .def_rw("D", &XmpAreaStruct::D)
        .def_rw("Unit", &XmpAreaStruct::Unit);

    nb::class_<DimensionsStruct>(m, "DimensionsStruct")
        .def(nb::init<double, double, const std::string&>(), "H"_a, "W"_a,
            "Unit"_a)
        .def("__eq__",
            [](const DimensionsStruct& self, const DimensionsStruct& other)
            { return self == other; })
        .def_rw("H", &DimensionsStruct::H)
        .def_rw("W", &DimensionsStruct::W)
        .def_rw("Unit", &DimensionsStruct::Unit);

    nb::class_<RegionStruct>(m, "RegionStruct")
        .def(nb::init<const XmpAreaStruct&, const std::string&,
                 const std::string&, std::optional<std::string>>(),
            "Area"_a, "Name"_a, "Type"_a, "Description"_a = nb::none())
        .def("__eq__", [](const RegionStruct& self, const RegionStruct& other)
            { return self == other; })
        .def_rw("Area", &RegionStruct::Area)
        .def_rw("Name", &RegionStruct::Name)
        .def_rw("Type", &RegionStruct::Type)
        .def_rw("Description", &RegionStruct::Description);

    nb::class_<RegionInfoStruct>(m, "RegionInfoStruct")
        .def(nb::init<const DimensionsStruct&,
                 const std::vector<RegionStruct>&>(),
            "AppliedToDimensions"_a, "RegionList"_a)
        .def("__eq__",
            [](const RegionInfoStruct& self, const RegionInfoStruct& other)
            { return self == other; })
        .def_rw("AppliedToDimensions", &RegionInfoStruct::AppliedToDimensions)
        .def_rw("RegionList", &RegionInfoStruct::RegionList);

    nb::class_<KeywordStruct>(m, "KeywordStruct")
        .def(nb::init<const std::string&, const std::vector<KeywordStruct>&,
                 std::optional<bool>>(),
            "Keyword"_a, "Children"_a, "Applied"_a = nb::none())
        .def("__eq__", [](const KeywordStruct& self, const KeywordStruct& other)
            { return self == other; })
        .def_rw("Keyword", &KeywordStruct::Keyword)
        .def_rw("Applied", &KeywordStruct::Applied)
        .def_rw("Children", &KeywordStruct::Children);

    nb::class_<KeywordInfoModel>(m, "KeywordInfoModel")
        .def(nb::init<const std::vector<KeywordStruct>&>(), "Hierarchy"_a)
        .def("__eq__",
            [](const KeywordInfoModel& self, const KeywordInfoModel& other)
            { return self == other; })
        .def_rw("Hierarchy", &KeywordInfoModel::Hierarchy);

    m.def("read_metadata", &read_metadata, "Read metadata from an image file");
    m.def("write_metadata", &write_metadata, "Write metadata to an image file");
    m.def("clear_existing_metadata", &clear_existing_metadata,
        "Clear existing metadata from an image file");
}
