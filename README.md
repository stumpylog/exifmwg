# EXIF Metadata Working Group Tool

[![PyPI - Version](https://img.shields.io/pypi/v/exifmwg.svg)](https://pypi.org/project/exifmwg)
[![PyPI - Python Version](https://img.shields.io/pypi/pyversions/exifmwg.svg)](https://pypi.org/project/exifmwg)
[![codecov](https://codecov.io/github/stumpylog/exifmwg/branch/main/graph/badge.svg?token=PTESS6YUK5)](https://codecov.io/github/stumpylog/exifmwg)

---

## Table of Contents

- [What](#what)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
  - [Quick Start](#quick-start)
  - [Reading and Modifying Image Regions](#reading-and-modifying-image-regions)
  - [Working with Keyword Hierarchies](#working-with-keyword-hierarchies)
- [Documentation](#documentation)
- [Why](#why)
- [License](#license)

## What

This library is a wrapper around [PyExifTool](https://github.com/sylikc/pyexiftool) to simplify the reading and writing of image metadata fields as defined by the [Metadata Working Group](https://en.wikipedia.org/wiki/Metadata_Working_Group).
The specification [Guidelines for Handling Image Metadata (PDF)](https://web.archive.org/web/20120131102845/http://www.metadataworkinggroup.org/pdf/mwg_guidance.pdf) defines a structured way to resolve certain common fields
across the various versions and standards for image metadata.

Working with image metadata is challenging due to the multiple competing standards and implementation differences. This library abstracts away these complexities, providing a consistent interface for accessing and modifying metadata regardless of which standard is used in the image.

## Features

- **Simplified Metadata Management**: Directly links to [Exiv2](https://exiv2.org/) to read or set the common MWG fields
- **Strongly Typed Interface**: All functions, properties and returns are typed
- **Key Metadata Fields**:
  - [`XMP-mwg-kw:KeywordInfo`](https://exiftool.org/TagNames/MWG.html#KeywordInfo) - Read and write hierarchical keyword/tag trees for images
  - [`XMP-mwg-rs:RegionInfo`](https://exiftool.org/TagNames/MWG.html#RegionInfo) - Manage region annotations for images (face tagging, object recognition boxes)
  - `MWG:Description` - Access standardized image descriptions across metadata formats
  - `Title` - Manage image titles consistently
- **Extensive testing**: Tested via both Python and C++ unit tests, with code coverage, including branch coverage information

## Requirements

- Python 3.9 or higher

## Installation

```console
pip install exifmwg
```

## Usage

### Quick Start

```python
from pathlib import Path
from exifmwg import read_metadata
from exifmwg import write_metadata

# Read metadata from an image
metadata = read_metadata(Path("sample.jpg"))
# Print the image title
print(f"Image title: {metadata.Title}")

# Set a new title and description
metadata.Title = "Sunset at the Beach"
metadata.Description = "Beautiful sunset captured at Malibu Beach"
write_metadata(metadata)
```

### Reading and Modifying Image Regions

```python
from pathlib import Path
from exifmwg import read_metadata
from exifmwg import write_metadata

test_file = Path("sample.jpeg")

# Read metadata from image
metadata = tool.read_image_metadata(test_file)

# Print the defined regions and their type for an image
for region in metadata.RegionInfo.RegionList:
    print(f"Name: {region.Name}, Type: {region.Type}, Description: {region.Description or '(None)'}")

# Change the person's name in the first region
metadata.RegionInfo.RegionList[0].Name = "Billy Bob"
tool.write_image_metadata(metadata)
```

Example output:

```console
Name: John Smith, Type: Face, Description: Family member
Name: Rover, Type: Pet, Description: Family dog
```

### Working with Keyword Hierarchies

```python
from pathlib import Path
from exifmwg import Keyword
from exifmwg import read_metadata
from exifmwg import write_metadata

test_file = Path("sample.jpeg")

metadata = read_metadata(test_file)

# Create a new keyword hierarchy
new_keyword = Keyword(
    Keyword="Vacation",
    Children=[
        models.Keyword(Keyword="Beach"),
        models.Keyword(Keyword="Mountain")
    ]
)

# Add to existing keywords
metadata.KeywordInfo.KeywordList.append(new_keyword)

# Write back to the image
write_metadata(metadata)
```

## Documentation

For more detailed information about using this library, refer to:

- [API Documentation](#) (This is still a TODO item)
- [ExifTool MWG Tags](https://exiftool.org/TagNames/MWG.html)

## Why

Although this could all be done directly with PyExifTool, using this wrapper provides several advantages:

1. Strongly typed interface makes it easier to understand what fields are available
2. Simplified API reduces the learning curve for managing image metadata
3. Handles the complexity of different metadata standards and their implementations

## License

`exifmwg` is distributed under the terms of the [Mozilla Public License 2.0](https://spdx.org/licenses/MPL-2.0.html) license.
