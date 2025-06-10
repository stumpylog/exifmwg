#pragma once

#include <filesystem>

#include "ImageMetadata.hpp"

// Main metadata writing function
void write_metadata(const std::filesystem::path &filepath,
                    const ImageMetadata &metadata);
