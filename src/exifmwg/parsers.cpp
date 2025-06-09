#include "parsers.hpp"

#include <stdexcept>

#include "utils.hpp"

XmpAreaStruct
parse_area_struct(const Exiv2::XmpData& xmpData, const std::string& baseKey)
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
    const Exiv2::XmpData& xmpData,
    const std::string&    baseKey)
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
    const Exiv2::XmpData& xmpData,
    const std::string&    key,
    char                  delimiter)
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

KeywordStruct
parse_keyword_struct(const Exiv2::XmpData& xmpData, const std::string& basePath)
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
        applied =
            (appliedValue == "True" || appliedValue == "true" ||
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
