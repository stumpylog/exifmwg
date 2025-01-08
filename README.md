# Tika Rest Client

[![PyPI - Version](https://img.shields.io/pypi/v/exif-mwg.svg)](https://pypi.org/project/exif-mwg)
[![PyPI - Python Version](https://img.shields.io/pypi/pyversions/exif-mwg.svg)](https://pypi.org/project/exif-mwg)
[![codecov](https://codecov.io/github/stumpylog/exif-mwg/branch/main/graph/badge.svg?token=PTESS6YUK5)](https://codecov.io/github/stumpylog/exif-mwg)

---

## Table of Contents

- [What](#what)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Why](#why)
- [License](#license)

## What

This library is a wrapper around [PyExifTool](https://github.com/sylikc/pyexiftool) to simplify the reading and writing of image metadata fields as defined by the [Metadata Working Group](https://en.wikipedia.org/wiki/Metadata_Working_Group).
The specification [Guidelines for Handling Image Metadata (PDF)](https://web.archive.org/web/20120131102845/http://www.metadataworkinggroup.org/pdf/mwg_guidance.pdf) defines a structured way to resolve certain common fields
across the various versions and standards for image metadata.

## Features

- Wrapping PyExifTool so a single process is started and managed
- Supports reading and writing a number of metadata fields, in a strongly typed way (thanks to Pydantic)
- Most important fields include:
  - [`XMP-mwg-kw:KeywordInfo`](https://exiftool.org/TagNames/MWG.html#KeywordInfo) - A list of tree structures to read or write the tag tree of an image
  - [`XMP-mwg-rs:RegionInfo](https://exiftool.org/TagNames/MWG.html#RegionInfo) - A list of regions defined for the image, to mark faces and pets with applied boxes
  - `MWG:Description` - Reads or sets the image description
  - `Title` - Reads or sets the image title

## Installation

```console
pip3 install exif-mwg
```

## Usage

```python3
from pathlib import Path
from exif_mwg import ExifTool

test_file = Path("sample.jpeg")


with ExifTool() as tool:

    # Print the defined regions and their type for an image
    metadata = tool.read_image_metadata(test_file)
    for region in metadata.RegionInfo.RegionList:
        print(region.Name, region.Type, region.Description or "")

    # Change the person's name
    metadata.RegionInfo.RegionList[0].Name = "Billy Bob"
    tool.write_image_metadata(metadata)


```

## Why

Although this could all be done directly with PyExifTool, wrapping it into a nice interface, with typed returns and easy to set attributes, instead of needing to remember what XMP value or other to set.

## License

`exif-mwg` is distributed under the terms of the [Mozilla Public License 2.0](https://spdx.org/licenses/MPL-2.0.html) license.
