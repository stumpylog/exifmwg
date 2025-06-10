#pragma once

#include <string>
#include <vector>

#include "models.hpp"

#include "exiv2/exiv2.hpp"

// String parsing utilities
std::vector<std::string> parse_delimited_string(const Exiv2::XmpData &xmpData,
                                                const std::string &key,
                                                char delimiter = ',');
