# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed

- Transitions to using a Python native module, linked against Exiv2
  - Uses Exiv2 v0.28.5
  - Uses libexpat 2.7.1
  - Known limitations
    - PNG files are not supported on Windows (yet), thanks to zlib linking problems

## [0.1.0] - 2025-04-08

### Added

- Initial version released
