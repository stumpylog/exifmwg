#include "ImageMetadata.hpp"

ImageMetadata::ImageMetadata(int imageHeight, int imageWidth, std::optional<std::string> title,
                             std::optional<std::string> description, std::optional<RegionInfoStruct> regionInfo,
                             std::optional<int> orientation, std::optional<std::vector<std::string>> lastKeywordXMP,
                             std::optional<std::vector<std::string>> tagsList,
                             std::optional<std::vector<std::string>> catalogSets,
                             std::optional<std::vector<std::string>> hierarchicalSubject,
                             std::optional<KeywordInfoModel> keywordInfo, std::optional<std::string> country,
                             std::optional<std::string> city, std::optional<std::string> state,
                             std::optional<std::string> location) :
    ImageHeight(imageHeight), ImageWidth(imageWidth), Title(title), Description(description), RegionInfo(regionInfo),
    Orientation(orientation), LastKeywordXMP(lastKeywordXMP), TagsList(tagsList), CatalogSets(catalogSets),
    HierarchicalSubject(hierarchicalSubject), KeywordInfo(keywordInfo), Country(country), City(city), State(state),
    Location(location) {
}

bool operator==(const ImageMetadata& lhs, const ImageMetadata& rhs) {
  return (lhs.ImageHeight == rhs.ImageHeight) && (lhs.ImageWidth == rhs.ImageWidth) && (lhs.Title == rhs.Title) &&
         (lhs.Description == rhs.Description) && (lhs.RegionInfo == rhs.RegionInfo) &&
         (lhs.Orientation == rhs.Orientation) && (lhs.LastKeywordXMP == rhs.LastKeywordXMP) &&
         (lhs.TagsList == rhs.TagsList) && (lhs.CatalogSets == rhs.CatalogSets) &&
         (lhs.HierarchicalSubject == rhs.HierarchicalSubject) && (lhs.KeywordInfo == rhs.KeywordInfo) &&
         (lhs.Country == rhs.Country) && (lhs.City == rhs.City) && (lhs.State == rhs.State) &&
         (lhs.Location == rhs.Location);
}

bool operator!=(const ImageMetadata& lhs, const ImageMetadata& rhs) {
  return !(lhs == rhs);
}
