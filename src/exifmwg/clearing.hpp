#pragma once

#include "exiv2/exiv2.hpp"
#include <filesystem>
#include <string>
void clear_existing_metadata(const std::filesystem::path &filepath);
