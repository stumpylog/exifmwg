import os
import pathlib
from collections.abc import Sequence
from typing import overload

class ImageMetadata:
    def __init__(
        self,
        image_height: int,
        image_width: int,
        title: str | None = None,
        description: str | None = None,
        region_info: RegionInfo | None = None,
        orientation: int | None = None,
        last_keyword_xmp: Sequence[str] | None = None,
        tags_list: Sequence[str] | None = None,
        catalog_sets: Sequence[str] | None = None,
        hierarchical_subject: Sequence[str] | None = None,
        keyword_info: KeywordInfo | None = None,
        country: str | None = None,
        city: str | None = None,
        state: str | None = None,
        location: str | None = None,
    ) -> None: ...
    def __eq__(self, arg: ImageMetadata, /) -> bool: ...
    def __ne__(self, arg: ImageMetadata, /) -> bool: ...
    @property
    def image_height(self) -> int: ...
    @image_height.setter
    def image_height(self, arg: int, /) -> None: ...
    @property
    def image_width(self) -> int: ...
    @image_width.setter
    def image_width(self, arg: int, /) -> None: ...
    @property
    def title(self) -> str | None: ...
    @title.setter
    def title(self, arg: str, /) -> None: ...
    @property
    def description(self) -> str | None: ...
    @description.setter
    def description(self, arg: str, /) -> None: ...
    @property
    def region_info(self) -> RegionInfo | None: ...
    @region_info.setter
    def region_info(self, arg: RegionInfo, /) -> None: ...
    @property
    def orientation(self) -> int | None: ...
    @orientation.setter
    def orientation(self, arg: int, /) -> None: ...
    @property
    def last_keyword_xmp(self) -> list[str] | None: ...
    @last_keyword_xmp.setter
    def last_keyword_xmp(self, arg: Sequence[str], /) -> None: ...
    @property
    def tags_list(self) -> list[str] | None: ...
    @tags_list.setter
    def tags_list(self, arg: Sequence[str], /) -> None: ...
    @property
    def catalog_sets(self) -> list[str] | None: ...
    @catalog_sets.setter
    def catalog_sets(self, arg: Sequence[str], /) -> None: ...
    @property
    def hierarchical_subject(self) -> list[str] | None: ...
    @hierarchical_subject.setter
    def hierarchical_subject(self, arg: Sequence[str], /) -> None: ...
    @property
    def keyword_info(self) -> KeywordInfo | None: ...
    @keyword_info.setter
    def keyword_info(self, arg: KeywordInfo, /) -> None: ...
    @property
    def country(self) -> str | None: ...
    @country.setter
    def country(self, arg: str, /) -> None: ...
    @property
    def city(self) -> str | None: ...
    @city.setter
    def city(self, arg: str, /) -> None: ...
    @property
    def state(self) -> str | None: ...
    @state.setter
    def state(self, arg: str, /) -> None: ...
    @property
    def location(self) -> str | None: ...
    @location.setter
    def location(self, arg: str, /) -> None: ...

class Image:
    """
    Represents an image file and provides methods for reading, writing, and clearing its metadata (e.g., XMP, EXIF). Metadata fields are exposed as properties that can be directly accessed and modified. Changes to these properties are not saved until the `save()` method is called.
    """

    def __init__(self, file_path: str | os.PathLike) -> None:
        """Constructs an Image object by reading metadata from the given path."""

    def __eq__(self, arg: Image, /) -> bool:
        """Compares two Image objects for equality."""

    def __ne__(self, arg: Image, /) -> bool:
        """Compares two Image objects for inequality."""

    def save(self, new_path: str | os.PathLike | None = None) -> None:
        """
        Saves metadata changes back to a file. If `new_path` is provided, the original image is copied to the new location and the metadata is written to the new file. Otherwise, it overwrites the original file with the updated metadata.
        """

    def clear_metadata(self) -> None:
        """
        Clears all supported metadata fields from the object and saves the changes back to the original file. This is a destructive operation.
        """

    @property
    def path(self) -> pathlib.Path:
        """Get the path to the image file as a read-only property."""

    @property
    def image_height(self) -> int:
        """The height of the image in pixels."""

    @image_height.setter
    def image_height(self, arg: int, /) -> None: ...
    @property
    def image_width(self) -> int:
        """The width of the image in pixels."""

    @image_width.setter
    def image_width(self, arg: int, /) -> None: ...
    @property
    def title(self) -> str | None:
        """
        The intellectual graphic title for the image, often corresponding to `dc:title` in XMP.
        """

    @title.setter
    def title(self, arg: str, /) -> None: ...
    @property
    def description(self) -> str | None:
        """
        A textual description of the image, often corresponding to `dc:description` in XMP.
        """

    @description.setter
    def description(self, arg: str, /) -> None: ...
    @property
    def region_info(self) -> RegionInfo | None:
        """
        Information about regions (e.g., faces) within the image, including their dimensions, names, and types. Corresponds to `mwg-rs:Regions` in XMP.
        """

    @region_info.setter
    def region_info(self, arg: RegionInfo, /) -> None: ...
    @property
    def orientation(self) -> int | None:
        """
        The orientation of the image, typically an EXIF orientation tag (e.g., 1 for normal, 6 for rotated 90 degrees clockwise).
        """

    @orientation.setter
    def orientation(self, arg: int, /) -> None: ...
    @property
    def last_keyword_xmp(self) -> list[str] | None:
        """
        The last keyword applied to the image, often corresponding to `digiKam:TagsList` or similar in XMP, though its exact meaning can vary.
        """

    @last_keyword_xmp.setter
    def last_keyword_xmp(self, arg: Sequence[str], /) -> None: ...
    @property
    def tags_list(self) -> list[str] | None:
        """
        A list of tags or keywords associated with the image, often corresponding to `dc:subject` or `photoshop:SupplementalCategories` in XMP.
        """

    @tags_list.setter
    def tags_list(self, arg: Sequence[str], /) -> None: ...
    @property
    def catalog_sets(self) -> list[str] | None:
        """
        The catalog sets or categories the image belongs to, typically used in photo management software. Corresponds to `microsoft:CatalogSets` in XMP.
        """

    @catalog_sets.setter
    def catalog_sets(self, arg: Sequence[str], /) -> None: ...
    @property
    def hierarchical_subject(self) -> list[str] | None:
        """
        Hierarchical keywords or subjects, representing a categorized structure for keywords. Corresponds to `lr:hierarchicalSubject` in XMP.
        """

    @hierarchical_subject.setter
    def hierarchical_subject(self, arg: Sequence[str], /) -> None: ...
    @property
    def keyword_info(self) -> KeywordInfo | None:
        """
        Detailed keyword information, potentially including hierarchical structures and application status. This is a more structured representation than `tags_list` or `hierarchical_subject`.
        """

    @keyword_info.setter
    def keyword_info(self, arg: KeywordInfo, /) -> None: ...
    @property
    def country(self) -> str | None:
        """
        The country where the image was taken or is associated with. Corresponds to `Iptc4xmpExt:CountryName` or `photoshop:Country` in XMP.
        """

    @country.setter
    def country(self, arg: str, /) -> None: ...
    @property
    def city(self) -> str | None:
        """
        The city where the image was taken or is associated with. Corresponds to `Iptc4xmpCore:City` or `photoshop:City` in XMP.
        """

    @city.setter
    def city(self, arg: str, /) -> None: ...
    @property
    def state(self) -> str | None:
        """
        The state/province where the image was taken or is associated with. Corresponds to `Iptc4xmpCore:ProvinceState` or `photoshop:State` in XMP.
        """

    @state.setter
    def state(self, arg: str, /) -> None: ...
    @property
    def location(self) -> str | None:
        """
        A general location description for the image, often more specific than just city/state/country. Corresponds to `Iptc4xmpCore:Location` or `photoshop:Location` in XMP.
        """

    @location.setter
    def location(self, arg: str, /) -> None: ...

class XmpArea:
    def __init__(self, h: float, w: float, x: float, y: float, unit: str, d: float | None = None) -> None: ...
    def __eq__(self, arg: XmpArea, /) -> bool: ...
    @property
    def h(self) -> float: ...
    @h.setter
    def h(self, arg: float, /) -> None: ...
    @property
    def w(self) -> float: ...
    @w.setter
    def w(self, arg: float, /) -> None: ...
    @property
    def x(self) -> float: ...
    @x.setter
    def x(self, arg: float, /) -> None: ...
    @property
    def y(self) -> float: ...
    @y.setter
    def y(self, arg: float, /) -> None: ...
    @property
    def d(self) -> float | None: ...
    @d.setter
    def d(self, arg: float, /) -> None: ...
    @property
    def unit(self) -> str: ...
    @unit.setter
    def unit(self, arg: str, /) -> None: ...

class Dimensions:
    def __init__(self, h: float, w: float, unit: str) -> None: ...
    def __eq__(self, arg: Dimensions, /) -> bool: ...
    @property
    def h(self) -> float: ...
    @h.setter
    def h(self, arg: float, /) -> None: ...
    @property
    def w(self) -> float: ...
    @w.setter
    def w(self, arg: float, /) -> None: ...
    @property
    def unit(self) -> str: ...
    @unit.setter
    def unit(self, arg: str, /) -> None: ...

class Region:
    def __init__(self, area: XmpArea, name: str, type_: str, description: str | None = None) -> None: ...
    def __eq__(self, arg: Region, /) -> bool: ...
    @property
    def area(self) -> XmpArea: ...
    @area.setter
    def area(self, arg: XmpArea, /) -> None: ...
    @property
    def name(self) -> str: ...
    @name.setter
    def name(self, arg: str, /) -> None: ...
    @property
    def type(self) -> str: ...
    @type.setter
    def type(self, arg: str, /) -> None: ...
    @property
    def description(self) -> str | None: ...
    @description.setter
    def description(self, arg: str, /) -> None: ...

class RegionInfo:
    def __init__(self, applied_to_dimensions: Dimensions, region_list: Sequence[Region]) -> None: ...
    def __eq__(self, arg: RegionInfo, /) -> bool: ...
    @property
    def applied_to_dimensions(self) -> Dimensions: ...
    @applied_to_dimensions.setter
    def applied_to_dimensions(self, arg: Dimensions, /) -> None: ...
    @property
    def region_list(self) -> list[Region]: ...
    @region_list.setter
    def region_list(self, arg: Sequence[Region], /) -> None: ...

class Keyword:
    def __init__(self, keyword: str, children: Sequence[Keyword], applied: bool | None = None) -> None: ...
    def __eq__(self, arg: Keyword, /) -> bool: ...
    @property
    def keyword(self) -> str: ...
    @keyword.setter
    def keyword(self, arg: str, /) -> None: ...
    @property
    def applied(self) -> bool | None: ...
    @applied.setter
    def applied(self, arg: bool, /) -> None: ...
    @property
    def children(self) -> list[Keyword]: ...
    @children.setter
    def children(self, arg: Sequence[Keyword], /) -> None: ...

class KeywordInfo:
    @overload
    def __init__(self, hierarchy: Sequence[Keyword]) -> None: ...
    @overload
    def __init__(self, delimited_strings: Sequence[str]) -> None: ...
    @overload
    def __init__(self, delimited_strings: Sequence[str], delimiter: str) -> None: ...
    def __eq__(self, arg: KeywordInfo, /) -> bool: ...
    def __or__(self, arg: KeywordInfo, /) -> KeywordInfo: ...
    def __ior__(self, arg: KeywordInfo, /) -> KeywordInfo: ...
    @property
    def hierarchy(self) -> list[Keyword]: ...
    @hierarchy.setter
    def hierarchy(self, arg: Sequence[Keyword], /) -> None: ...

def read_metadata(arg: str | os.PathLike, /) -> ImageMetadata:
    """Read metadata from an image file"""

def write_metadata(arg0: str | os.PathLike, arg1: ImageMetadata, /) -> None:
    """Write metadata to an image file"""

def clear_existing_metadata(arg: str | os.PathLike, /) -> None:
    """Clear existing metadata from an image file"""

def exiv2_version() -> str:
    """Returns the Exiv2 library version string."""

def expat_version() -> str:
    """Returns the libexpat library version string."""
