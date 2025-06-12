#include "ImageMetadata.hpp"

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

/**
 * @brief Checks equality for two ImageMetadata objects.
 *
 * Compares all metadata fields between two ImageMetadata instances for equality.
 *
 * @param lhs The left-hand side ImageMetadata object.
 * @param rhs The right-hand side ImageMetadata object.
 * @return true if all fields are equal; false otherwise.
 */
bool operator==(const ImageMetadata& lhs, const ImageMetadata& rhs) {
  return (lhs.ImageHeight == rhs.ImageHeight) && (lhs.ImageWidth == rhs.ImageWidth) && (lhs.Title == rhs.Title) &&
         (lhs.Description == rhs.Description) && (lhs.RegionInfo == rhs.RegionInfo) &&
         (lhs.Orientation == rhs.Orientation) && (lhs.LastKeywordXMP == rhs.LastKeywordXMP) &&
         (lhs.TagsList == rhs.TagsList) && (lhs.CatalogSets == rhs.CatalogSets) &&
         (lhs.HierarchicalSubject == rhs.HierarchicalSubject) && (lhs.KeywordInfo == rhs.KeywordInfo) &&
         (lhs.Country == rhs.Country) && (lhs.City == rhs.City) && (lhs.State == rhs.State) &&
         (lhs.Location == rhs.Location);
}

/**
 * @brief Checks inequality for two ImageMetadata objects.
 *
 * Inverse of the equality operator; returns true if any field differs between the two objects.
 *
 * @param lhs The left-hand side ImageMetadata object.
 * @param rhs The right-hand side ImageMetadata object.
 * @return true if any field differs; false otherwise.
 */
bool operator!=(const ImageMetadata& lhs, const ImageMetadata& rhs) {
  return !(lhs == rhs);
}
