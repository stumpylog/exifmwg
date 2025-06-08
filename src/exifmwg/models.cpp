#include "models.hpp"

// XmpAreaStruct
XmpAreaStruct::XmpAreaStruct(double h, double w, double x, double y,
    const std::string& unit, std::optional<double> d)
    : H(h), W(w), X(x), Y(y), Unit(unit), D(d)
{
}

// DimensionsStruct
DimensionsStruct::DimensionsStruct(double h, double w, const std::string& unit)
    : H(h), W(w), Unit(unit)
{
}

// RegionStruct
RegionStruct::RegionStruct(const XmpAreaStruct& area, const std::string& name,
    const std::string& type, std::optional<std::string> description)
    : Area(area), Name(name), Type(type), Description(description)
{
}

// RegionInfoStruct
RegionInfoStruct::RegionInfoStruct(const DimensionsStruct& appliedToDimensions,
    const std::vector<RegionStruct>&                       regionList)
    : AppliedToDimensions(appliedToDimensions), RegionList(regionList)
{
}

// KeywordStruct
KeywordStruct::KeywordStruct(const std::string& keyword,
    const std::vector<KeywordStruct>& children, std::optional<bool> applied)
    : Keyword(keyword), Applied(applied), Children(children)
{
}

// KeywordInfoModel
KeywordInfoModel::KeywordInfoModel(const std::vector<KeywordStruct>& hierarchy)
    : Hierarchy(hierarchy)
{
}

// ImageMetadata
ImageMetadata::ImageMetadata(const fs::path& sourceFile, int imageHeight,
    int imageWidth, std::optional<std::string> title,
    std::optional<std::string>      description,
    std::optional<RegionInfoStruct> regionInfo, std::optional<int> orientation,
    std::optional<std::vector<std::string>> lastKeywordXMP,
    std::optional<std::vector<std::string>> tagsList,
    std::optional<std::vector<std::string>> catalogSets,
    std::optional<std::vector<std::string>> hierarchicalSubject,
    std::optional<KeywordInfoModel>         keywordInfo,
    std::optional<std::string> country, std::optional<std::string> city,
    std::optional<std::string> state, std::optional<std::string> location)
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
