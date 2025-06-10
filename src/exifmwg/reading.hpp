#pragma once

#include <filesystem>

#include "ImageMetadata.hpp"

ImageMetadata read_metadata(const std::filesystem::path &filepath);
