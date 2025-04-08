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

- **Simplified Metadata Management**: Wraps PyExifTool in a convenient interface, managing a single process for better performance
- **Strongly Typed Interface**: Leverages Pydantic models for type checking and validation of metadata fields
- **Cross-Standard Support**: Instructs ExifTool to use MWG guidelines to normalize access to metadata across different standards
- **Key Metadata Fields**:
  - [`XMP-mwg-kw:KeywordInfo`](https://exiftool.org/TagNames/MWG.html#KeywordInfo) - Read and write hierarchical keyword/tag trees for images
  - [`XMP-mwg-rs:RegionInfo`](https://exiftool.org/TagNames/MWG.html#RegionInfo) - Manage region annotations for images (face tagging, object recognition boxes)
  - `MWG:Description` - Access standardized image descriptions across metadata formats
  - `Title` - Manage image titles consistently

## Requirements

- Python 3.9 or higher
- [ExifTool](https://exiftool.org/) installed and available in your system path
  - On Ubuntu/Debian: `apt-get install exiftool`
  - On MacOS: `brew install exiftool`
  - On Windows: Download and install from the [official site](https://exiftool.org/)

## Installation

```console
pip install exifmwg
```

## Usage

### Quick Start

```python
from pathlib import Path
from exifmwg import ExifTool

# Read metadata from an image
with ExifTool() as tool:
    metadata = tool.read_image_metadata(Path("sample.jpg"))
    # Print the image title
    print(f"Image title: {metadata.Title}")

    # Set a new title and description
    metadata.Title = "Sunset at the Beach"
    metadata.Description = "Beautiful sunset captured at Malibu Beach"
    tool.write_image_metadata(metadata)
```

### Reading and Modifying Image Regions

```python
from pathlib import Path
from exifmwg import ExifTool

test_file = Path("sample.jpeg")

with ExifTool() as tool:
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
from exifmwg import ExifTool, models

test_file = Path("sample.jpeg")

with ExifTool() as tool:
    metadata = tool.read_image_metadata(test_file)

    # Create a new keyword hierarchy
    new_keyword = models.Keyword(
        Keyword="Vacation",
        Children=[
            models.Keyword(Keyword="Beach"),
            models.Keyword(Keyword="Mountain")
        ]
    )

    # Add to existing keywords
    metadata.KeywordInfo.KeywordList.append(new_keyword)

    # Write back to the image
    tool.write_image_metadata(metadata)
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
