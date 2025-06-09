#include "models.hpp"
#include "utils.hpp"

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

// XmpAreaStruct
XmpAreaStruct::XmpAreaStruct(
    double                h,
    double                w,
    double                x,
    double                y,
    const std::string&    unit,
    std::optional<double> d)
    : H(h), W(w), X(x), Y(y), Unit(unit), D(d)
{
}

// DimensionsStruct
DimensionsStruct::DimensionsStruct(double h, double w, const std::string& unit)
    : H(h), W(w), Unit(unit)
{
}

// RegionStruct
RegionStruct::RegionStruct(
    const XmpAreaStruct&       area,
    const std::string&         name,
    const std::string&         type,
    std::optional<std::string> description)
    : Area(area), Name(name), Type(type), Description(description)
{
}

// RegionInfoStruct
RegionInfoStruct::RegionInfoStruct(
    const DimensionsStruct&          appliedToDimensions,
    const std::vector<RegionStruct>& regionList)
    : AppliedToDimensions(appliedToDimensions), RegionList(regionList)
{
}

// KeywordStruct
KeywordStruct::KeywordStruct(
    const std::string&                keyword,
    const std::vector<KeywordStruct>& children,
    std::optional<bool>               applied)
    : Keyword(keyword), Applied(applied), Children(children)
{
}

// KeywordInfoModel
KeywordInfoModel::KeywordInfoModel(const std::vector<KeywordStruct>& hierarchy)
    : Hierarchy(hierarchy)
{
}

// ImageMetadata
ImageMetadata::ImageMetadata(
    const fs::path&                         sourceFile,
    int                                     imageHeight,
    int                                     imageWidth,
    std::optional<std::string>              title,
    std::optional<std::string>              description,
    std::optional<RegionInfoStruct>         regionInfo,
    std::optional<int>                      orientation,
    std::optional<std::vector<std::string>> lastKeywordXMP,
    std::optional<std::vector<std::string>> tagsList,
    std::optional<std::vector<std::string>> catalogSets,
    std::optional<std::vector<std::string>> hierarchicalSubject,
    std::optional<KeywordInfoModel>         keywordInfo,
    std::optional<std::string>              country,
    std::optional<std::string>              city,
    std::optional<std::string>              state,
    std::optional<std::string>              location)
    : SourceFile(sourceFile), ImageHeight(imageHeight), ImageWidth(imageWidth),
      Title(title), Description(description), RegionInfo(regionInfo),
      Orientation(orientation), LastKeywordXMP(lastKeywordXMP),
      TagsList(tagsList), CatalogSets(catalogSets),
      HierarchicalSubject(hierarchicalSubject), KeywordInfo(keywordInfo),
      Country(country), City(city), State(state), Location(location)
{
}

// Equality operators
bool operator==(const XmpAreaStruct& lhs, const XmpAreaStruct& rhs)
{
    return lhs.H == rhs.H && lhs.W == rhs.W && lhs.X == rhs.X &&
           lhs.Y == rhs.Y && lhs.Unit == rhs.Unit && lhs.D == rhs.D;
}

bool operator==(const DimensionsStruct& lhs, const DimensionsStruct& rhs)
{
    return lhs.H == rhs.H && lhs.W == rhs.W && lhs.Unit == rhs.Unit;
}

bool operator==(const RegionStruct& lhs, const RegionStruct& rhs)
{
    return lhs.Area == rhs.Area && lhs.Name == rhs.Name &&
           lhs.Type == rhs.Type && lhs.Description == rhs.Description;
}

bool operator==(const RegionInfoStruct& lhs, const RegionInfoStruct& rhs)
{
    return lhs.AppliedToDimensions == rhs.AppliedToDimensions &&
           lhs.RegionList == rhs.RegionList;
}

bool operator==(const KeywordStruct& lhs, const KeywordStruct& rhs)
{
    return lhs.Keyword == rhs.Keyword && lhs.Applied == rhs.Applied &&
           lhs.Children == rhs.Children;
}

bool operator==(const KeywordInfoModel& lhs, const KeywordInfoModel& rhs)
{
    return lhs.Hierarchy == rhs.Hierarchy;
}

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

KeywordStruct *findOrCreateChild(
    std::vector<KeywordStruct>& children,
    const std::string&          keyword)
{
    for (auto& child : children)
    {
        if (child.Keyword == keyword)
        {
            return &child;
        }
    }

    // Create new child if not found using existing constructor
    children.emplace_back(keyword, std::vector<KeywordStruct>{}, std::nullopt);
    return &children.back();
}

KeywordInfoModel buildHierarchyFromStrings(
    const std::vector<std::string>& delimitedStrings,
    char                            delimiter = '/')
{
    std::vector<KeywordStruct> rootNodes;

    for (const std::string& delimitedString : delimitedStrings)
    {
        std::vector<std::string> pathTokens =
            split_string(delimitedString, delimiter);

        if (pathTokens.empty())
        {
            continue;
        }

        // Start at root level
        std::vector<KeywordStruct> *currentLevel = &rootNodes;

        for (const std::string& token : pathTokens)
        {
            KeywordStruct *node = findOrCreateChild(*currentLevel, token);
            currentLevel        = &(node->Children);
        }
    }

    return KeywordInfoModel(rootNodes);
}

// Helper function to merge Applied values
std::optional<bool>
mergeApplied(const std::optional<bool>& a, const std::optional<bool>& b)
{
    if (!a.has_value())
        return b;
    if (!b.has_value())
        return a;
    // Both have values - prioritize 'true' over 'false'
    return a.value() || b.value();
}

// Recursive function to merge keyword vectors
std::vector<KeywordStruct> mergeKeywordVectors(
    const std::vector<KeywordStruct>& vec1,
    const std::vector<KeywordStruct>& vec2)
{

    std::vector<KeywordStruct> result;

    // Add all keywords from vec1
    for (const auto& keyword1 : vec1)
    {
        // Look for matching keyword in vec2
        auto it = std::find_if(
            vec2.begin(),
            vec2.end(),
            [&keyword1](const KeywordStruct& k)
            { return k.Keyword == keyword1.Keyword; });

        if (it != vec2.end())
        {
            // Found matching keyword - merge them
            std::vector<KeywordStruct> mergedChildren =
                mergeKeywordVectors(keyword1.Children, it->Children);

            result.emplace_back(
                keyword1.Keyword,
                mergedChildren,
                mergeApplied(keyword1.Applied, it->Applied));
        }
        else
        {
            // No match found - add keyword1 as-is
            result.push_back(keyword1);
        }
    }

    // Add keywords from vec2 that weren't already processed
    for (const auto& keyword2 : vec2)
    {
        auto it = std::find_if(
            vec1.begin(),
            vec1.end(),
            [&keyword2](const KeywordStruct& k)
            { return k.Keyword == keyword2.Keyword; });

        if (it == vec1.end())
        {
            // keyword2 not found in vec1 - add it
            result.push_back(keyword2);
        }
    }

    return result;
}

KeywordInfoModel mergeKeywordInfoModels(
    const KeywordInfoModel& model1,
    const KeywordInfoModel& model2)
{

    std::vector<KeywordStruct> mergedHierarchy =
        mergeKeywordVectors(model1.Hierarchy, model2.Hierarchy);

    return KeywordInfoModel(mergedHierarchy);
}

KeywordInfoModel& KeywordInfoModel::operator|=(const KeywordInfoModel& other)
{
    this->Hierarchy = mergeKeywordVectors(this->Hierarchy, other.Hierarchy);
    return *this;
}

KeywordInfoModel
KeywordInfoModel::operator|(const KeywordInfoModel& other) const
{
    return KeywordInfoModel(
        mergeKeywordVectors(this->Hierarchy, other.Hierarchy));
}
