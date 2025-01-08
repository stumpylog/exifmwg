# SPDX-FileCopyrightText: 2025-present Trenton H <rda0128ou@mozmail.com>
#
# SPDX-License-Identifier: MPL-2.0

from __future__ import annotations

import enum
from typing import Literal

from pydantic import BaseModel
from pydantic import Field
from pydantic import FilePath
from pydantic import field_serializer


@enum.unique
class RotationEnum(enum.IntEnum):
    """
    https://exiftool.org/TagNames/EXIF.html (0x0112)
    """

    HORIZONTAL = 1
    MIRROR_HORIZONTAL = 2
    ROTATE_180 = 3
    MIRROR_VERTICAL = 4
    MIRROR_HORIZONTAL_AND_ROTATE_270_CW = 5
    ROTATE_90_CW = 6
    MIRROR_HORIZONTAL_AND_ROTATE_90_CW = 7
    ROTATE_270_CW = 8


class XmpAreaStruct(BaseModel):
    """
    https://exiftool.org/TagNames/XMP.html#Area
    """

    H: float
    Unit: Literal["normalized", "pixel"]
    W: float
    X: float
    Y: float
    D: float | None = None


class DimensionsStruct(BaseModel):
    """
    https://exiftool.org/TagNames/XMP.html#Dimensions
    """

    H: float
    W: float
    Unit: Literal["pixel", "inch"]


class RegionStruct(BaseModel):
    """
    https://exiftool.org/TagNames/MWG.html#RegionStruct
    """

    Area: XmpAreaStruct
    Name: str
    Type: Literal["BarCode", "Face", "Focus", "Pet"]
    Description: str | None = None


class RegionInfoStruct(BaseModel):
    """
    https://exiftool.org/TagNames/MWG.html#RegionInfo
    """

    AppliedToDimensions: DimensionsStruct
    RegionList: list[RegionStruct]


class KeywordStruct(BaseModel):
    """
    https://exiftool.org/TagNames/MWG.html#KeywordStruct
    """

    Keyword: str
    Applied: bool | None = None
    Children: list[KeywordStruct] = Field(default_factory=list)

    def __hash__(self) -> int:
        """
        Don't hash the children, consider this unique if the keyword and applied state are the same
        """
        return hash(self.Keyword) + hash(self.Applied)

    def get_child_by_name(self, name: str) -> KeywordStruct | None:
        """
        Helper to retrieve a child by the given name, if it exists
        """
        name = name.lower()
        for child in self.Children:
            if child.Keyword.lower() == name:
                return child
        return None


# Recursive, so rebuild
KeywordStruct.model_rebuild()


class KeywordInfoModel(BaseModel):
    """
    https://exiftool.org/TagNames/MWG.html#KeywordInfo
    """

    Hierarchy: list[KeywordStruct]

    def get_root_by_name(self, name: str) -> KeywordStruct | None:
        """
        Locates the given root node by the given name, if it exists
        """
        name = name.lower()
        for keyword in self.Hierarchy:
            if keyword.Keyword.lower() == name:
                return keyword
        return None


class ImageMetadata(BaseModel):
    """
    Defines the possible fields which can be read or set from an image using exiftool
    at this point
    """

    SourceFile: FilePath = Field(description="The source or destination of the metadata")

    ImageHeight: int = Field(description="Height of the image in pixel", default=-1)
    ImageWidth: int = Field(description="Width of the image in pixel", default=-1)

    Title: str | None = None
    Description: str | None = Field(default=None, description="Reads or sets the MWG:Description")

    RegionInfo: RegionInfoStruct | None = Field(
        default=None,
        description="Reads or sets the XMP-mwg-rs:RegionInfo",
    )
    Orientation: RotationEnum | None = Field(default=None, description="Reads or sets the MWG:Orientation")

    LastKeywordXMP: list[str] | None = Field(
        default=None,
        description="Reads or sets the XMP-microsoft:LastKeywordXMP",
    )
    TagsList: list[str] | None = Field(default=None, description="Reads or sets the XMP-digiKam:TagsList")
    CatalogSets: list[str] | None = Field(
        default=None,
        description="Reads or sets the IPTC:CatalogSets or XMP-mediapro:CatalogSets",
    )
    HierarchicalSubject: list[str] | None = Field(
        default=None,
        description="Reads or sets the XMP-lr:HierarchicalSubject",
    )
    KeywordInfo: KeywordInfoModel | None = Field(
        default=None,
        description=(
            "Reads or sets the XMP-mwg-kw:KeywordInfo.  This is the preferred method to set keywords"
            " and will override other values"
        ),
    )

    Country: str | None = Field(default=None, description="Reads or sets the MWG:Country")
    City: str | None = Field(default=None, description="Reads or sets the MWG:City")
    State: str | None = Field(default=None, description="Reads or sets the MWG:State")
    Location: str | None = Field(default=None, description="Reads or sets the MWG:Location")

    @field_serializer("SourceFile")  # type: ignore[misc]
    def serialize_source_file(self, source_file: FilePath, _info) -> str:  # type: ignore[no-untyped-def]
        """
        Somethings fails to understand Path, so return the fully resolved path as a string
        """
        return str(source_file.resolve())
