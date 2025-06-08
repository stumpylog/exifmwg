from __future__ import annotations

import difflib
from typing import TYPE_CHECKING
from typing import Any
from typing import Optional

import pytest

if TYPE_CHECKING:
    # Updated imports and type hints for new class names
    from exifmwg import ImageMetadata
    from exifmwg import Keyword
    from exifmwg import KeywordInfo
    from exifmwg import RegionInfo


def _generate_string_diff(expected: str, actual: str) -> str:
    """Generates an indented unified diff for two strings."""
    diff_lines = difflib.unified_diff(
        expected.splitlines(keepends=True),
        actual.splitlines(keepends=True),
        fromfile="expected",
        tofile="actual",
    )
    # Indent the diff for better readability in the final pytest error message
    return "\n" + "".join(f"      {line}" for line in diff_lines)


def _compare_field(errors: list[str], field_name: str, expected: Any, actual: Any):
    """
    Compares a field and appends a detailed error to the list on mismatch.
    Provides a string diff for non-matching string values.
    """
    if expected == actual:
        return

    message = f"{field_name} mismatch"
    if isinstance(expected, str) and isinstance(actual, str):
        # For multiline or long strings, a diff is much clearer.
        message += f":{_generate_string_diff(expected, actual)}"
    else:
        # For other types, show a clear representation of both values.
        message += f":\n  - Expected: {expected!r}\n  - Actual:   {actual!r}"
    errors.append(message)


def compare_lists(field_name: str, expected: Optional[list], actual: Optional[list]) -> list[str]:
    """Compare optional lists and return a list of detailed error messages."""
    errors: list[str] = []
    if expected is None and actual is None:
        return errors
    if expected is None or actual is None:
        _compare_field(errors, field_name, expected, actual)
        return errors

    if len(expected) != len(actual):
        errors.append(f"{field_name} length mismatch: expected {len(expected)}, got {len(actual)}")
        return errors

    for i, (exp_item, act_item) in enumerate(zip(expected, actual)):
        _compare_field(errors, f"{field_name}[{i}]", exp_item, act_item)

    return errors


def compare_region_info(expected: Optional[RegionInfo], actual: Optional[RegionInfo]) -> list[str]:
    """Compare RegionInfo structures and return detailed error messages."""
    errors: list[str] = []

    if expected is None and actual is None:
        return errors
    if expected is None or actual is None:
        # Changed attribute name to snake_case
        _compare_field(errors, "region_info", expected, actual)
        return errors

    # Compare AppliedToDimensions - Changed attribute access to snake_case
    exp_dims = expected.applied_to_dimensions
    act_dims = actual.applied_to_dimensions
    _compare_field(errors, "region_info.applied_to_dimensions.h", exp_dims.h, act_dims.h)
    _compare_field(errors, "region_info.applied_to_dimensions.w", exp_dims.w, act_dims.w)
    _compare_field(errors, "region_info.applied_to_dimensions.unit", exp_dims.unit, act_dims.unit)

    # Compare RegionList - Changed attribute access to snake_case
    if len(expected.region_list) != len(actual.region_list):
        errors.append(
            f"region_info.region_list length mismatch: "
            f"expected {len(expected.region_list)}, got {len(actual.region_list)}",
        )
        return errors

    for i, (exp_region, act_region) in enumerate(zip(expected.region_list, actual.region_list)):
        prefix = f"region_list[{i}]"
        # Compare Area - Changed attribute access to snake_case
        exp_area, act_area = exp_region.area, act_region.area
        _compare_field(errors, f"{prefix}.area.h", exp_area.h, act_area.h)
        _compare_field(errors, f"{prefix}.area.w", exp_area.w, act_area.w)
        _compare_field(errors, f"{prefix}.area.x", exp_area.x, act_area.x)
        _compare_field(errors, f"{prefix}.area.y", exp_area.y, act_area.y)
        _compare_field(errors, f"{prefix}.area.unit", exp_area.unit, act_area.unit)
        _compare_field(errors, f"{prefix}.area.d", exp_area.d, act_area.d)

        # Compare other region fields - Changed attribute access to snake_case
        _compare_field(errors, f"{prefix}.name", exp_region.name, act_region.name)
        _compare_field(errors, f"{prefix}.type", exp_region.type, act_region.type)
        _compare_field(errors, f"{prefix}.description", exp_region.description, act_region.description)

    return errors


def compare_keyword_info(expected: Optional[KeywordInfo], actual: Optional[KeywordInfo]) -> list[str]:
    """Compare KeywordInfo structures and return detailed error messages."""
    errors: list[str] = []
    if expected is None and actual is None:
        return errors
    if expected is None or actual is None:
        # Changed attribute name to snake_case
        _compare_field(errors, "keyword_info", expected, actual)
        return errors

    def _compare_keyword_struct(exp_kw: Keyword, act_kw: Keyword, path: str):
        kw_errors: list[str] = []
        kw_path = f"keyword_info{path}"
        # Changed attribute access to snake_case
        _compare_field(kw_errors, f"{kw_path}.keyword", exp_kw.keyword, act_kw.keyword)
        _compare_field(kw_errors, f"{kw_path}.applied", exp_kw.applied, act_kw.applied)

        if len(exp_kw.children) != len(act_kw.children):
            kw_errors.append(
                f"{kw_path}.children length mismatch: expected {len(exp_kw.children)}, got {len(act_kw.children)}",
            )
        else:
            for i, (exp_child, act_child) in enumerate(zip(exp_kw.children, act_kw.children)):
                kw_errors.extend(_compare_keyword_struct(exp_child, act_child, f"{path}.children[{i}]"))
        return kw_errors

    if len(expected.hierarchy) != len(actual.hierarchy):
        errors.append(
            f"keyword_info.hierarchy length mismatch: expected {len(expected.hierarchy)}, got {len(actual.hierarchy)}",
        )
    else:
        for i, (exp_kw, act_kw) in enumerate(zip(expected.hierarchy, actual.hierarchy)):
            errors.extend(_compare_keyword_struct(exp_kw, act_kw, f".hierarchy[{i}]"))

    return errors


def verify_expected_vs_actual_metadata(expected: ImageMetadata, actual: ImageMetadata):
    """
    Compare two ImageMetadata instances and fail with detailed error messages.

    Args:
        expected: The expected ImageMetadata instance.
        actual: The actual ImageMetadata instance.

    Raises:
        pytest.fail: A detailed error message if any fields don't match.
    """
    errors: list[str] = []

    # Basic fields - Changed attribute access to snake_case
    _compare_field(errors, "source_file", expected.source_file, actual.source_file)
    _compare_field(errors, "image_height", expected.image_height, actual.image_height)
    _compare_field(errors, "image_width", expected.image_width, actual.image_width)

    # Optional string fields - Changed attribute access to snake_case
    _compare_field(errors, "title", expected.title, actual.title)
    _compare_field(errors, "description", expected.description, actual.description)
    _compare_field(errors, "orientation", expected.orientation, actual.orientation)
    _compare_field(errors, "country", expected.country, actual.country)
    _compare_field(errors, "city", expected.city, actual.city)
    _compare_field(errors, "state", expected.state, actual.state)
    _compare_field(errors, "location", expected.location, actual.location)

    # List fields - Changed attribute names to snake_case
    errors.extend(compare_lists("last_keyword_xmp", expected.last_keyword_xmp, actual.last_keyword_xmp))
    errors.extend(compare_lists("tags_list", expected.tags_list, actual.tags_list))
    errors.extend(compare_lists("catalog_sets", expected.catalog_sets, actual.catalog_sets))
    errors.extend(compare_lists("hierarchical_subject", expected.hierarchical_subject, actual.hierarchical_subject))

    # Complex structures - Changed attribute names to snake_case
    errors.extend(compare_region_info(expected.region_info, actual.region_info))
    errors.extend(compare_keyword_info(expected.keyword_info, actual.keyword_info))

    # If any errors were found, fail the test with a detailed, formatted message
    if errors:
        error_message = f"ImageMetadata comparison failed with {len(errors)} error(s):\n"
        error_message += "\n".join(f"  - {error}" for error in errors)
        pytest.fail(error_message)
