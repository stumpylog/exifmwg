#pragma once

#include "models.hpp"
#include <filesystem>

ImageMetadata read_metadata(const std::filesystem::path &filepath);
