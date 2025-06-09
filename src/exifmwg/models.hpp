#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct XmpAreaStruct
{
    double                H;
    double                W;
    double                X;
    double                Y;
    std::string           Unit;
    std::optional<double> D;

    XmpAreaStruct(
        double                h,
        double                w,
        double                x,
        double                y,
        const std::string&    unit,
        std::optional<double> d);
};

struct DimensionsStruct
{
    double      H;
    double      W;
    std::string Unit;

    DimensionsStruct(double h, double w, const std::string& unit);
};

struct RegionStruct
{
    XmpAreaStruct              Area;
    std::string                Name;
    std::string                Type;
    std::optional<std::string> Description;

    RegionStruct(
        const XmpAreaStruct&       area,
        const std::string&         name,
        const std::string&         type,
        std::optional<std::string> description);
};

struct RegionInfoStruct
{
    DimensionsStruct          AppliedToDimensions;
    std::vector<RegionStruct> RegionList;

    RegionInfoStruct(
        const DimensionsStruct&          appliedToDimensions,
        const std::vector<RegionStruct>& regionList);
};

struct KeywordStruct
{
    std::string                Keyword;
    std::optional<bool>        Applied;
    std::vector<KeywordStruct> Children;

    KeywordStruct(
        const std::string&                keyword,
        const std::vector<KeywordStruct>& children,
        std::optional<bool>               applied);
};

struct KeywordInfoModel
{
    std::vector<KeywordStruct> Hierarchy;

    KeywordInfoModel(const std::vector<KeywordStruct>& hierarchy);
    KeywordInfoModel(
        const std::vector<std::string>& delimitedStrings,
        char                            delimiter = '/');

    KeywordInfoModel& operator|=(const KeywordInfoModel& other);
    KeywordInfoModel  operator|(const KeywordInfoModel& other) const;
};

struct ImageMetadata
{
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

    ImageMetadata(
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
        std::optional<std::string>              location);
};

// Equality operators
bool operator==(const XmpAreaStruct& lhs, const XmpAreaStruct& rhs);
bool operator==(const DimensionsStruct& lhs, const DimensionsStruct& rhs);
bool operator==(const RegionStruct& lhs, const RegionStruct& rhs);
bool operator==(const RegionInfoStruct& lhs, const RegionInfoStruct& rhs);
bool operator==(const KeywordStruct& lhs, const KeywordStruct& rhs);
bool operator==(const KeywordInfoModel& lhs, const KeywordInfoModel& rhs);
bool operator==(const ImageMetadata& lhs, const ImageMetadata& rhs);
