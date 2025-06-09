from __future__ import annotations

import os
import pathlib
from typing import TYPE_CHECKING
from typing import Any

if TYPE_CHECKING:
    from exifmwg import Dimensions
    from exifmwg import ImageMetadata
    from exifmwg import Keyword
    from exifmwg import KeywordInfo
    from exifmwg import Region
    from exifmwg import RegionInfo
    from exifmwg import XmpArea


class ComparisonResult:
    """Holds the result of a comparison between expected and actual values."""

    def __init__(self):
        self.differences: list[str] = []
        self.passed = True

    def add_difference(self, field_path: str, expected: Any, actual: Any):
        """Add a difference to the result."""
        self.passed = False
        self.differences.append(f"{field_path}: expected {expected!r}, got {actual!r}")

    def merge(self, other: ComparisonResult, prefix: str = ""):
        """Merge another comparison result into this one."""
        if not other.passed:
            self.passed = False
            for diff in other.differences:
                self.differences.append(f"{prefix}.{diff}" if prefix else diff)

    def __bool__(self):
        return self.passed

    def __str__(self):
        if self.passed:
            return "All comparisons passed"
        return f"Found {len(self.differences)} difference(s):\n" + "\n".join(f"  - {diff}" for diff in self.differences)


def compare_basic_values(field_name: str, expected: Any, actual: Any) -> ComparisonResult:
    """Compare basic values, handling None cases."""
    result = ComparisonResult()

    # Handle None cases
    if expected is None and actual is None:
        return result
    if expected is None and actual is not None:
        result.add_difference(field_name, expected, actual)
        return result
    if expected is not None and actual is None:
        result.add_difference(field_name, expected, actual)
        return result

    # Handle pathlib.Path comparison
    if isinstance(expected, (str, os.PathLike)) and isinstance(actual, pathlib.Path):
        if pathlib.Path(expected) != actual:
            result.add_difference(field_name, expected, actual)
    elif isinstance(actual, (str, os.PathLike)) and isinstance(expected, pathlib.Path):
        if expected != pathlib.Path(actual):
            result.add_difference(field_name, expected, actual)
    elif expected != actual:
        result.add_difference(field_name, expected, actual)

    return result


def compare_sequences(field_name: str, expected: list | None, actual: list | None) -> ComparisonResult:
    """Compare sequences/lists, handling None cases."""
    result = ComparisonResult()

    # Handle None cases
    if expected is None and actual is None:
        return result
    if expected is None and actual is not None:
        result.add_difference(field_name, expected, actual)
        return result
    if expected is not None and actual is None:
        result.add_difference(field_name, expected, actual)
        return result

    if TYPE_CHECKING:
        assert expected is not None
        assert actual is not None

    # Compare lengths
    if len(expected) != len(actual):
        result.add_difference(f"{field_name}.length", len(expected), len(actual))
        return result

    # Compare elements
    for i, (exp_item, act_item) in enumerate(zip(expected, actual)):
        if exp_item != act_item:
            result.add_difference(f"{field_name}[{i}]", exp_item, act_item)

    return result


def assert_xmp_area_equal(expected: XmpArea, actual: XmpArea) -> ComparisonResult:
    """Compare two XmpArea objects."""
    result = ComparisonResult()

    if expected is None and actual is None:
        return result
    if expected is None or actual is None:
        result.add_difference("XmpArea", expected, actual)
        return result

    result.merge(compare_basic_values("h", expected.h, actual.h))
    result.merge(compare_basic_values("w", expected.w, actual.w))
    result.merge(compare_basic_values("x", expected.x, actual.x))
    result.merge(compare_basic_values("y", expected.y, actual.y))
    result.merge(compare_basic_values("unit", expected.unit, actual.unit))
    result.merge(compare_basic_values("d", expected.d, actual.d))

    return result


def assert_dimensions_equal(expected: Dimensions, actual: Dimensions) -> ComparisonResult:
    """Compare two Dimensions objects."""
    result = ComparisonResult()

    if expected is None and actual is None:
        return result
    if expected is None or actual is None:
        result.add_difference("Dimensions", expected, actual)
        return result

    result.merge(compare_basic_values("h", expected.h, actual.h))
    result.merge(compare_basic_values("w", expected.w, actual.w))
    result.merge(compare_basic_values("unit", expected.unit, actual.unit))

    return result


def assert_region_equal(expected: Region, actual: Region) -> ComparisonResult:
    """Compare two Region objects."""
    result = ComparisonResult()

    if expected is None and actual is None:
        return result
    if expected is None or actual is None:
        result.add_difference("Region", expected, actual)
        return result

    result.merge(assert_xmp_area_equal(expected.area, actual.area), "area")
    result.merge(compare_basic_values("name", expected.name, actual.name))
    result.merge(compare_basic_values("type", expected.type, actual.type))
    result.merge(compare_basic_values("description", expected.description, actual.description))

    return result


def assert_region_info_equal(expected: RegionInfo | None, actual: RegionInfo | None) -> ComparisonResult:
    """Compare two RegionInfo objects."""
    result = ComparisonResult()

    if expected is None and actual is None:
        return result
    if expected is None or actual is None:
        result.add_difference("RegionInfo", expected, actual)
        return result

    result.merge(
        assert_dimensions_equal(expected.applied_to_dimensions, actual.applied_to_dimensions),
        "applied_to_dimensions",
    )

    # Compare region lists
    exp_regions = expected.region_list
    act_regions = actual.region_list

    if len(exp_regions) != len(act_regions):
        result.add_difference("region_list.length", len(exp_regions), len(act_regions))
    else:
        for i, (exp_region, act_region) in enumerate(zip(exp_regions, act_regions)):
            region_result = assert_region_equal(exp_region, act_region)
            result.merge(region_result, f"region_list[{i}]")

    return result


def assert_keyword_equal(expected: Keyword, actual: Keyword) -> ComparisonResult:
    """Compare two Keyword objects."""
    result = ComparisonResult()

    if expected is None and actual is None:
        return result
    if expected is None or actual is None:
        result.add_difference("Keyword", expected, actual)
        return result

    result.merge(compare_basic_values("keyword", expected.keyword, actual.keyword))
    result.merge(compare_basic_values("applied", expected.applied, actual.applied))

    # Compare children
    exp_children = expected.children
    act_children = actual.children

    if len(exp_children) != len(act_children):
        result.add_difference("children.length", len(exp_children), len(act_children))
    else:
        for i, (exp_child, act_child) in enumerate(zip(exp_children, act_children)):
            child_result = assert_keyword_equal(exp_child, act_child)
            result.merge(child_result, f"children[{i}]")

    return result


def assert_keyword_info_equal(expected: KeywordInfo | None, actual: KeywordInfo | None) -> ComparisonResult:
    """Compare two KeywordInfo objects."""
    result = ComparisonResult()

    if expected is None and actual is None:
        return result
    if expected is None or actual is None:
        result.add_difference("KeywordInfo", expected, actual)
        return result

    # Compare hierarchy
    exp_hierarchy = expected.hierarchy
    act_hierarchy = actual.hierarchy

    if len(exp_hierarchy) != len(act_hierarchy):
        result.add_difference("hierarchy.length", len(exp_hierarchy), len(act_hierarchy))
    else:
        for i, (exp_keyword, act_keyword) in enumerate(zip(exp_hierarchy, act_hierarchy)):
            keyword_result = assert_keyword_equal(exp_keyword, act_keyword)
            result.merge(keyword_result, f"hierarchy[{i}]")

    return result


def assert_image_metadata_equal(expected: ImageMetadata, actual: ImageMetadata) -> ComparisonResult:
    """Compare two ImageMetadata objects comprehensively."""
    result = ComparisonResult()

    if expected is None and actual is None:
        return result
    if expected is None or actual is None:
        result.add_difference("ImageMetadata", expected, actual)
        return result

    # Compare basic properties
    result.merge(compare_basic_values("source_file", expected.source_file, actual.source_file))
    result.merge(compare_basic_values("image_height", expected.image_height, actual.image_height))
    result.merge(compare_basic_values("image_width", expected.image_width, actual.image_width))
    result.merge(compare_basic_values("title", expected.title, actual.title))
    result.merge(compare_basic_values("description", expected.description, actual.description))
    result.merge(compare_basic_values("orientation", expected.orientation, actual.orientation))
    result.merge(compare_basic_values("country", expected.country, actual.country))
    result.merge(compare_basic_values("city", expected.city, actual.city))
    result.merge(compare_basic_values("state", expected.state, actual.state))
    result.merge(compare_basic_values("location", expected.location, actual.location))

    # Compare sequence properties
    result.merge(compare_sequences("last_keyword_xmp", expected.last_keyword_xmp, actual.last_keyword_xmp))
    result.merge(compare_sequences("tags_list", expected.tags_list, actual.tags_list))
    result.merge(compare_sequences("catalog_sets", expected.catalog_sets, actual.catalog_sets))
    result.merge(compare_sequences("hierarchical_subject", expected.hierarchical_subject, actual.hierarchical_subject))

    # Compare complex objects
    result.merge(assert_region_info_equal(expected.region_info, actual.region_info), "region_info")
    result.merge(assert_keyword_info_equal(expected.keyword_info, actual.keyword_info), "keyword_info")

    return result


# Convenience functions for easier testing
def verify_image_metadata(expected: ImageMetadata, actual: ImageMetadata, description: str = ""):
    """Verify ImageMetadata objects are equal, raising AssertionError with detailed info if not."""
    result = assert_image_metadata_equal(expected, actual)
    if not result:
        error_msg = "ImageMetadata comparison failed"
        if description:
            error_msg += f" ({description})"
        error_msg += f":\n{result}"
        raise AssertionError(error_msg)


def verify_region_info(expected: RegionInfo, actual: RegionInfo, description: str = ""):
    """Verify RegionInfo objects are equal, raising AssertionError with detailed info if not."""
    result = assert_region_info_equal(expected, actual)
    if not result:
        error_msg = "RegionInfo comparison failed"
        if description:
            error_msg += f" ({description})"
        error_msg += f":\n{result}"
        raise AssertionError(error_msg)


def verify_keyword_info(expected: KeywordInfo, actual: KeywordInfo, description: str = ""):
    """Verify KeywordInfo objects are equal, raising AssertionError with detailed info if not."""
    result = assert_keyword_info_equal(expected, actual)
    if not result:
        error_msg = "KeywordInfo comparison failed"
        if description:
            error_msg += f" ({description})"
        error_msg += f":\n{result}"
        raise AssertionError(error_msg)
