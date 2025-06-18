#pragma once

enum class ExifOrientation : int {
  // Set but not a valid value
  Undefined = 0,
  // Normal (0° rotation)
  Horizontal = 1,
  TopLeft = 1,
  // Horizontal flip
  MirrorHorizontal = 2,
  TopRight = 2,
  // 180° rotation
  Rotate180 = 3,
  BottomRight = 3,
  // Vertical flip
  MirrorVertical = 4,
  BottomLeft = 4,
  // 90° CCW rotation + horizontal flip
  MirrorHorizontalAndRotate270CW = 5,
  LeftTop = 5,
  // 90° CW rotation
  Rotate90CW = 6,
  RightTop = 6,
  // 90° CW rotation + horizontal flip
  MirrorHorizontalAndRotate90CW = 7,
  RightBottom = 7,
  // 90° CCW rotation
  Rotate270CW = 8,
  LeftBottom = 8
};

// Convert enum to int for exiv2 writing
constexpr int orientation_to_int(ExifOrientation orientation) noexcept {
  return static_cast<int>(orientation);
}

// Convert int to enum for exiv2 reading
constexpr ExifOrientation orientation_from_int(int value) noexcept {
  // Validate range and return Undefined for invalid values
  if (value >= 0 && value <= 8) {
    return static_cast<ExifOrientation>(value);
  }
  return ExifOrientation::Undefined;
}

// Convert enum to EXIF orientation value (alias for clarity)
constexpr int orientation_to_exif_value(ExifOrientation orientation) noexcept {
  return orientation_to_int(orientation);
}

// Convert EXIF orientation value to enum (alias for clarity)
constexpr ExifOrientation orientation_from_exif_value(int exif_value) noexcept {
  return orientation_from_int(exif_value);
}

// Check if orientation is valid (not undefined)
constexpr bool orientation_is_valid(ExifOrientation orientation) noexcept {
  return orientation != ExifOrientation::Undefined;
}

// Get string representation for debugging/logging
constexpr const char* orientation_to_string(ExifOrientation orientation) noexcept {
  switch (orientation) {
  case ExifOrientation::Undefined:
    return "Undefined";
  case ExifOrientation::Horizontal:
    return "Horizontal";
  case ExifOrientation::MirrorHorizontal:
    return "MirrorHorizontal";
  case ExifOrientation::Rotate180:
    return "Rotate180";
  case ExifOrientation::MirrorVertical:
    return "MirrorVertical";
  case ExifOrientation::MirrorHorizontalAndRotate270CW:
    return "MirrorHorizontalAndRotate270CW";
  case ExifOrientation::Rotate90CW:
    return "Rotate90CW";
  case ExifOrientation::MirrorHorizontalAndRotate90CW:
    return "MirrorHorizontalAndRotate90CW";
  case ExifOrientation::Rotate270CW:
    return "Rotate270CW";
  default:
    return "Unknown";
  }
}
