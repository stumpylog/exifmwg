#pragma once

#include <filesystem>
#include <string>
#include "exiv2/exiv2.hpp"
void clear_existing_metadata(const std::filesystem::path& filepath);

void clear_xmp_key(Exiv2::XmpData& xmpData, const std::string& baseKey);
