#pragma once
#include "models.hpp"
#include <filesystem>

// Main metadata writing function
void write_metadata(const std::filesystem::path &filepath,
                    const ImageMetadata &metadata);
