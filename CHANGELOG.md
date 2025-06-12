# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.2.0] - 2025-06-12

### Changed

- Transitions to using a Python native module, linked against Exiv2, an no longer relies on exiftool ([#3](https://github.com/stumpylog/exifmwg/pull/3))
  - Uses Exiv2 v0.28.5
  - Uses libexpat 2.7.1
  - Uses zlib 1.3.1
  - PNG files are not maybe supported on Windows (yet), thanks to zlib linking problems on that platform

## [0.1.0] - 2025-04-08

### Added

- Initial version released
