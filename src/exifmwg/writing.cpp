
#include <string>

#include "models.hpp"
#include "utils.hpp"
#include "writing.hpp"

#include "exiv2/exiv2.hpp"

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
