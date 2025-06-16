# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed

- Image metadata is now read and written through the single class `ImageMetadata` ([#7](https://github.com/stumpylog/exifmwg/pull/7))

### Removed

- For now, metadata clearing has been removed until a later date

## [0.3.0] - 2025-06-13

### Added

- C++ unit tests now include PNG and WebP images in at least a basic fashion

### Changed

- PNG support is firmly disable for Windows builds now

### Fixed

- Removed the debug printing to `stderr` for release builds
- Fixed an issue where images with no `Xmp.mwg-rs.Regions` key would still try to parse regions

## [0.2.0] - 2025-06-12

### Changed

- Transitions to using a Python native module, linked against Exiv2, an no longer relies on exiftool ([#3](https://github.com/stumpylog/exifmwg/pull/3))
  - Uses Exiv2 v0.28.5
  - Uses libexpat 2.7.1
  - Uses zlib 1.3.1
  - PNG files are not maybe supported on Windows (yet), thanks to zlib linking problems on that platform
- Bump pypa/cibuildwheel from 2.23.3 to 3.0.0 ([#5](https://github.com/stumpylog/exifmwg/pull/5))
- Bump codecov/codecov-action from 4 to 5 ([#4](https://github.com/stumpylog/exifmwg/pull/4))

## [0.1.0] - 2025-04-08

### Added

- Initial version released
