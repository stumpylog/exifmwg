#pragma once

#include "exiv2/exiv2.hpp"
#include "models.hpp"
#include <string>
#include <vector>

// Area and region parsing functions
XmpAreaStruct parse_area_struct(
    const Exiv2::XmpData& xmpData, const std::string& baseKey);

DimensionsStruct parse_dimensions_struct(
    const Exiv2::XmpData& xmpData, const std::string& baseKey);

RegionInfoStruct parse_region_info(const Exiv2::XmpData& xmpData);

// Keyword parsing functions
KeywordStruct parse_keyword_struct(
    const Exiv2::XmpData& xmpData, const std::string& basePath);

KeywordInfoModel parse_keyword_info(const Exiv2::XmpData& xmpData);

// String parsing utilities
std::vector<std::string> parse_delimited_string(const Exiv2::XmpData& xmpData,
    const std::string& key, char delimiter = ',');
