#include <string>

#include "clearing.hpp"
#include "models.hpp"
#include "utils.hpp"
#include "writing.hpp"
#include "exiv2/exiv2.hpp"

namespace fs = std::filesystem;

void write_keyword_struct(
    Exiv2::XmpData&      xmpData,
    const KeywordStruct& keywordStruct,
    const std::string&   basePath)
{
    // Write the keyword
    xmpData[basePath + "/mwg-kw:Keyword"] = keywordStruct.Keyword;

    // Write Applied attribute if present
    if (keywordStruct.Applied)
    {
        xmpData[basePath + "/mwg-kw:Applied"] =
            *keywordStruct.Applied ? "True" : "False";
    }

    // Write children recursively
    for (size_t i = 0; i < keywordStruct.Children.size(); ++i)
    {
        xmpData[basePath + "/mwg-kw:Children"] = "";
        std::string childPath =
            basePath + "/mwg-kw:Children[" + std::to_string(i + 1) + "]";
        write_keyword_struct(xmpData, keywordStruct.Children[i], childPath);
    }
}

void write_keyword_info(
    Exiv2::XmpData&         xmpData,
    const KeywordInfoModel& keywordInfo)
{
    LOG_DEBUG("Writing MWG Keywords hierarchy");

    // Clear existing MWG Keywords data
    clear_xmp_key(xmpData, "Xmp.mwg-kw.Keywords");

    if (keywordInfo.Hierarchy.empty())
    {
        return;
    }

    const std::string basePath = "Xmp.mwg-kw.Keywords/mwg-kw:Hierarchy";
    xmpData[basePath]          = "";

    // Write hierarchy items
    for (size_t i = 0; i < keywordInfo.Hierarchy.size(); ++i)
    {
        std::string itemPath = basePath + "[" + std::to_string(i + 1) + "]";
        write_keyword_struct(xmpData, keywordInfo.Hierarchy[i], itemPath);
    }

    LOG_DEBUG(
        "Wrote " + std::to_string(keywordInfo.Hierarchy.size()) +
        " top-level keyword hierarchy items");
}

/**
 * @brief Writes an XmpAreaStruct to the given XMP data object.
 *
 * @param xmpData The XMP data object to write to.
 * @param area The XmpAreaStruct to write.
 * @param basePath The base XMP path for the area data.
 */
void write_area_struct(
    Exiv2::XmpData&      xmpData,
    const XmpAreaStruct& area,
    const std::string&   basePath)
{
    xmpData[basePath + "/stArea:h"]    = doubleToStringWithPrecision(area.H);
    xmpData[basePath + "/stArea:w"]    = doubleToStringWithPrecision(area.W);
    xmpData[basePath + "/stArea:x"]    = doubleToStringWithPrecision(area.X);
    xmpData[basePath + "/stArea:y"]    = doubleToStringWithPrecision(area.Y);
    xmpData[basePath + "/stArea:unit"] = area.Unit;

    if (area.D)
    {
        xmpData[basePath + "/stArea:d"] = std::to_string(*area.D);
    }
}

/**
 * @brief Writes a DimensionsStruct to the given XMP data object.
 *
 * @param xmpData The XMP data object to write to.
 * @param dims The DimensionsStruct to write.
 * @param basePath The base XMP path for the dimensions data.
 */
void write_dimensions_struct(
    Exiv2::XmpData&         xmpData,
    const DimensionsStruct& dims,
    const std::string&      basePath)
{
    std::ostringstream oss;
    oss << std::setprecision(10);
    oss << dims.H;
    xmpData[basePath + "/stDim:h"] = oss.str();
    oss.str("");
    oss.clear();
    oss << dims.W;
    xmpData[basePath + "/stDim:w"]    = oss.str();
    xmpData[basePath + "/stDim:unit"] = dims.Unit;
}

/**
 * @brief Writes the complete MWG Regions structure to XMP metadata.
 *
 * @param xmpData The XMP data object to write to.
 * @param regionInfo The RegionInfoStruct containing the region data.
 */
void write_region_info(
    Exiv2::XmpData&         xmpData,
    const RegionInfoStruct& regionInfo)
{
    LOG_DEBUG("Writing MWG Regions hierarchy");

    // Clear existing MWG Regions data
    clear_xmp_key(xmpData, "Xmp.mwg-rs.Regions");

    // Write AppliedToDimensions first
    std::string appliedToDimPath =
        "Xmp.mwg-rs.Regions/mwg-rs:AppliedToDimensions";
    write_dimensions_struct(
        xmpData, regionInfo.AppliedToDimensions, appliedToDimPath);

    const std::string baseRegionList = "Xmp.mwg-rs.Regions/mwg-rs:RegionList";
    xmpData[baseRegionList]          = "";

    // Write regions if any exist
    for (size_t i = 0; i < regionInfo.RegionList.size(); ++i)
    {
        const auto&       region = regionInfo.RegionList[i];
        const std::string itemPath =
            baseRegionList + "[" + std::to_string(i + 1) + "]";

        // Write the Area struct
        std::string areaPath = itemPath + "/mwg-rs:Area";
        write_area_struct(xmpData, region.Area, areaPath);

        // Write other region properties
        xmpData[itemPath + "/mwg-rs:Name"] = region.Name;
        xmpData[itemPath + "/mwg-rs:Type"] = region.Type;

        if (region.Description)
        {
            xmpData[itemPath + "/mwg-rs:Description"] = *region.Description;
        }
    }

    LOG_DEBUG(
        "Wrote " + std::to_string(regionInfo.RegionList.size()) + " regions.");
}

void write_metadata(const fs::path& filepath, const ImageMetadata& metadata)
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
        if (metadata.State)
        {
            iptcData["Iptc.Application2.ProvinceState"] = *metadata.State;
            xmpData["Xmp.photoshop.State"]              = *metadata.State;
        }
        if (metadata.City)
        {
            iptcData["Iptc.Application2.City"] = *metadata.City;
            xmpData["Xmp.photoshop.City"]      = *metadata.City;
        }
        if (metadata.Location)
        {
            iptcData["Iptc.Application2.SubLocation"] = *metadata.Location;
            xmpData["Xmp.iptc.Location"]              = *metadata.Location;
        }

        // Write MWG Regions
        if (metadata.RegionInfo)
        {
            write_region_info(xmpData, *metadata.RegionInfo);
        }

        // Write MWG Keywords
        if (metadata.KeywordInfo)
        {
            write_keyword_info(xmpData, *metadata.KeywordInfo);
        }

        // Write keyword arrays as comma-delimited strings
        if (metadata.LastKeywordXMP)
        {
            clear_xmp_key(xmpData, "Xmp.MicrosoftPhoto.LastKeywordXMP");
            // Write as comma-delimited string
            std::string combined = "";
            for (size_t i = 0; i < metadata.LastKeywordXMP->size(); ++i)
            {
                if (i > 0)
                    combined += ",";
                combined += (*metadata.LastKeywordXMP)[i];
            }
            xmpData["Xmp.MicrosoftPhoto.LastKeywordXMP"] = combined;
        }

        if (metadata.TagsList)
        {
            clear_xmp_key(xmpData, "Xmp.digiKam.TagsList");

            std::string combined = "";
            for (size_t i = 0; i < metadata.TagsList->size(); ++i)
            {
                if (i > 0)
                    combined += ",";
                combined += (*metadata.TagsList)[i];
            }
            xmpData["Xmp.digiKam.TagsList"] = combined;
        }

        if (metadata.CatalogSets)
        {
            clear_xmp_key(xmpData, "Xmp.mediapro.CatalogSets");
            std::string combined = "";
            for (size_t i = 0; i < metadata.CatalogSets->size(); ++i)
            {
                if (i > 0)
                    combined += ",";
                combined += (*metadata.CatalogSets)[i];
            }
            xmpData["Xmp.mediapro.CatalogSets"] = combined;
        }

        if (metadata.HierarchicalSubject)
        {
            clear_xmp_key(xmpData, "Xmp.lr.hierarchicalSubject");

            std::string combined = "";
            for (size_t i = 0; i < metadata.HierarchicalSubject->size(); ++i)
            {
                if (i > 0)
                    combined += ",";
                combined += (*metadata.HierarchicalSubject)[i];
            }
            xmpData["Xmp.lr.hierarchicalSubject"] = combined;
        }

        image->writeMetadata();
    }
    catch (const Exiv2::Error& e)
    {
        throw std::runtime_error("Exiv2 error: " + std::string(e.what()));
    }
}
