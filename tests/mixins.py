from __future__ import annotations

import difflib
from typing import TYPE_CHECKING
from typing import Any
from typing import Optional

import pytest

if TYPE_CHECKING:
    from exifmwg import ImageMetadata
    from exifmwg import KeywordInfoModel
    from exifmwg import KeywordStruct
    from exifmwg import RegionInfoStruct


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


def compare_region_info(expected: Optional[RegionInfoStruct], actual: Optional[RegionInfoStruct]) -> list[str]:
    """Compare RegionInfo structures and return detailed error messages."""
    errors: list[str] = []

    if expected is None and actual is None:
        return errors
    if expected is None or actual is None:
        _compare_field(errors, "RegionInfo", expected, actual)
        return errors

    # Compare AppliedToDimensions
    exp_dims = expected.AppliedToDimensions
    act_dims = actual.AppliedToDimensions
    _compare_field(errors, "RegionInfo.AppliedToDimensions.H", exp_dims.H, act_dims.H)
    _compare_field(errors, "RegionInfo.AppliedToDimensions.W", exp_dims.W, act_dims.W)
    _compare_field(errors, "RegionInfo.AppliedToDimensions.Unit", exp_dims.Unit, act_dims.Unit)

    # Compare RegionList
    if len(expected.RegionList) != len(actual.RegionList):
        errors.append(
            f"RegionInfo.RegionList length mismatch: expected {len(expected.RegionList)}, got {len(actual.RegionList)}",
        )
        return errors

    for i, (exp_region, act_region) in enumerate(zip(expected.RegionList, actual.RegionList)):
        prefix = f"RegionList[{i}]"
        # Compare Area
        exp_area, act_area = exp_region.Area, act_region.Area
        _compare_field(errors, f"{prefix}.Area.H", exp_area.H, act_area.H)
        _compare_field(errors, f"{prefix}.Area.W", exp_area.W, act_area.W)
        _compare_field(errors, f"{prefix}.Area.X", exp_area.X, act_area.X)
        _compare_field(errors, f"{prefix}.Area.Y", exp_area.Y, act_area.Y)
        _compare_field(errors, f"{prefix}.Area.Unit", exp_area.Unit, act_area.Unit)
        _compare_field(errors, f"{prefix}.Area.D", exp_area.D, act_area.D)

        # Compare other region fields
        _compare_field(errors, f"{prefix}.Name", exp_region.Name, act_region.Name)
        _compare_field(errors, f"{prefix}.Type", exp_region.Type, act_region.Type)
        _compare_field(errors, f"{prefix}.Description", exp_region.Description, act_region.Description)

    return errors


def compare_keyword_info(expected: Optional[KeywordInfoModel], actual: Optional[KeywordInfoModel]) -> list[str]:
    """Compare KeywordInfo structures and return detailed error messages."""
    errors: list[str] = []
    if expected is None and actual is None:
        return errors
    if expected is None or actual is None:
        _compare_field(errors, "KeywordInfo", expected, actual)
        return errors

    def _compare_keyword_struct(exp_kw: KeywordStruct, act_kw: KeywordStruct, path: str):
        kw_errors: list[str] = []
        kw_path = f"KeywordInfo{path}"
        _compare_field(kw_errors, f"{kw_path}.Keyword", exp_kw.Keyword, act_kw.Keyword)
        _compare_field(kw_errors, f"{kw_path}.Applied", exp_kw.Applied, act_kw.Applied)

        if len(exp_kw.Children) != len(act_kw.Children):
            kw_errors.append(
                f"{kw_path}.Children length mismatch: expected {len(exp_kw.Children)}, got {len(act_kw.Children)}",
            )
        else:
            for i, (exp_child, act_child) in enumerate(zip(exp_kw.Children, act_kw.Children)):
                kw_errors.extend(_compare_keyword_struct(exp_child, act_child, f"{path}.Children[{i}]"))
        return kw_errors

    if len(expected.Hierarchy) != len(actual.Hierarchy):
        errors.append(
            f"KeywordInfo.Hierarchy length mismatch: expected {len(expected.Hierarchy)}, got {len(actual.Hierarchy)}",
        )
    else:
        for i, (exp_kw, act_kw) in enumerate(zip(expected.Hierarchy, actual.Hierarchy)):
            errors.extend(_compare_keyword_struct(exp_kw, act_kw, f".Hierarchy[{i}]"))

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

    # Basic fields
    _compare_field(errors, "SourceFile", expected.SourceFile, actual.SourceFile)
    _compare_field(errors, "ImageHeight", expected.ImageHeight, actual.ImageHeight)
    _compare_field(errors, "ImageWidth", expected.ImageWidth, actual.ImageWidth)

    # Optional string fields
    _compare_field(errors, "Title", expected.Title, actual.Title)
    _compare_field(errors, "Description", expected.Description, actual.Description)
    _compare_field(errors, "Orientation", expected.Orientation, actual.Orientation)
    _compare_field(errors, "Country", expected.Country, actual.Country)
    _compare_field(errors, "City", expected.City, actual.City)
    _compare_field(errors, "State", expected.State, actual.State)
    _compare_field(errors, "Location", expected.Location, actual.Location)

    # List fields
    errors.extend(compare_lists("LastKeywordXMP", expected.LastKeywordXMP, actual.LastKeywordXMP))
    errors.extend(compare_lists("TagsList", expected.TagsList, actual.TagsList))
    errors.extend(compare_lists("CatalogSets", expected.CatalogSets, actual.CatalogSets))
    errors.extend(compare_lists("HierarchicalSubject", expected.HierarchicalSubject, actual.HierarchicalSubject))

    # Complex structures
    errors.extend(compare_region_info(expected.RegionInfo, actual.RegionInfo))
    errors.extend(compare_keyword_info(expected.KeywordInfo, actual.KeywordInfo))

    # If any errors were found, fail the test with a detailed, formatted message
    if errors:
        error_message = f"ImageMetadata comparison failed with {len(errors)} error(s):\n"
        error_message += "\n".join(f"  - {error}" for error in errors)
        pytest.fail(error_message)
