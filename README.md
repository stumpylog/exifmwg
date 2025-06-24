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
- [License](#license)

## What

This library is a wrapper around [Exiv2](https://exiv2.org/) to simplify the reading and writing of image metadata fields as defined by the [Metadata Working Group](https://en.wikipedia.org/wiki/Metadata_Working_Group).
The specification [Guidelines for Handling Image Metadata (PDF)](https://web.archive.org/web/20120131102845/http://www.metadataworkinggroup.org/pdf/mwg_guidance.pdf) defines a structured way to resolve certain common fields
across the various versions and standards for image metadata.

## Features

- **Simplified Metadata Management**: Directly links to [Exiv2](https://exiv2.org/) to read or set the common MWG fields
- **Strongly Typed Interface**: All functions, properties and returns are typed
- **Key Metadata Fields**:
  - [`XMP-mwg-kw:KeywordInfo`](https://exiftool.org/TagNames/MWG.html#KeywordInfo)
    - Read & write hierarchical keyword/tag trees, using both the MWG structures, and other XMP keys like `Xmp.digiKam.TagsList` for maximum compatibility across photo tools
  - [`XMP-mwg-rs:RegionInfo`](https://exiftool.org/TagNames/MWG.html#RegionInfo) - Manage region annotations for images (face tagging, object recognition boxes)
  - `MWG:Description` - Access standardized image descriptions across metadata formats
  - `Title` - Manage image titles consistently. Though not defined by MWG, it is often displayed by image programs
  - Location - Define the image location, using the Country, State, City and Sub-Location
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
from exifmwg import ImageMetadata

# Read metadata from an image
metadata = ImageMetadata(Path("sample.jpg"))
# Print the image title
print(f"Image title: {metadata.Title}")

# Set a new title and description
metadata.Title = "Sunset at the Beach"
metadata.Description = "Beautiful sunset captured at Malibu Beach"

# Save the updates to the original file
metadata.to_file()

# Save the updates to a new file
metadata.to_file(Path("updated_sample.jpg"))
```

### Reading and Modifying Image Regions

```python
from pathlib import Path
from exifmwg import ImageMetadata

test_file = Path("sample.jpeg")

# Read metadata from image
metadata = ImageMetadata(test_file)

# Print the defined regions and their type for an image
for region in metadata.RegionInfo.RegionList:
    print(f"Name: {region.Name}, Type: {region.Type}, Description: {region.Description or '(None)'}")

# Change the person's name in the first region
metadata.RegionInfo.RegionList[0].Name = "Billy Bob"
# Save the updates to the original file
metadata.to_file()
```

Example output:

```console
Name: John Smith, Type: Face, Description: Family member
Name: Rover, Type: Pet, Description: Family dog
```

### Working with Keyword Hierarchies

```python
from pathlib import Path
from exifmwg import ImageMetadata
from exifmwg import Keyword
from exifmwg import KeywordInfo

test_file = Path("sample.jpeg")

metadata = ImageMetadata(test_file)

# Create a new keyword hierarchy
new_keyword_tree = Keyword(
    Keyword="Vacation",
    Children=[
        models.Keyword(Keyword="Beach"),
        models.Keyword(Keyword="Mountain")
    ]
)

# Overwrite any existing keywords
metadata.keyword_info = KeywordInfo(hierarchy=[new_keyword_tree])

# Save the updates to the original file
metadata.to_file()
```

## Documentation

For more detailed information about using this library, refer to:

- [API Documentation](#) (This is still a TODO item)
- [ExifTool MWG Tags](https://exiftool.org/TagNames/MWG.html)

## License

`exifmwg` is distributed under the terms of the [Mozilla Public License 2.0](https://spdx.org/licenses/MPL-2.0.html) license.
