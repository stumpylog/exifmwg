#pragma once

#include <filesystem>
#include "models.hpp"

ImageMetadata read_metadata(const std::filesystem::path& filepath);
