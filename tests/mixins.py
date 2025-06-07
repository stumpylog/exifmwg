from __future__ import annotations

from typing import TYPE_CHECKING
from typing import List
from typing import Optional

import pytest

if TYPE_CHECKING:
    from exifmwg import ImageMetadata
    from exifmwg import KeywordInfoModel
    from exifmwg import RegionInfoStruct


def compare_optional_values(field_name: str, expected, actual):
    """Compare optional values and return error message if different."""
    if expected != actual:
        return f"{field_name} mismatch: expected {expected}, got {actual}"
    return None


def compare_lists(field_name: str, expected: Optional[List], actual: Optional[List]):
    """Compare optional lists and return detailed error message if different."""
    if expected is None and actual is None:
        return None
    if expected is None or actual is None:
        return f"{field_name} mismatch: expected {expected}, got {actual}"

    if len(expected) != len(actual):
        return f"{field_name} length mismatch: expected {len(expected)}, got {len(actual)}"

    for i, (exp_item, act_item) in enumerate(zip(expected, actual)):
        if exp_item != act_item:
            return f"{field_name}[{i}] mismatch: expected '{exp_item}', got '{act_item}'"

    return None


def compare_region_info(expected: Optional[RegionInfoStruct], actual: Optional[RegionInfoStruct]):
    """Compare RegionInfo structures and return detailed error messages."""
    errors = []

    if expected is None and actual is None:
        return errors
    if expected is None or actual is None:
        errors.append(f"RegionInfo mismatch: expected {expected}, got {actual}")
        return errors

    # Compare AppliedToDimensions
    exp_dims = expected.AppliedToDimensions
    act_dims = actual.AppliedToDimensions

    if exp_dims.H != act_dims.H:
        errors.append(f"RegionInfo.AppliedToDimensions.H mismatch: expected {exp_dims.H}, got {act_dims.H}")
    if exp_dims.W != act_dims.W:
        errors.append(f"RegionInfo.AppliedToDimensions.W mismatch: expected {exp_dims.W}, got {act_dims.W}")
    if exp_dims.Unit != act_dims.Unit:
        errors.append(
            f"RegionInfo.AppliedToDimensions.Unit mismatch: expected '{exp_dims.Unit}', got '{act_dims.Unit}'",
        )

    # Compare RegionList
    if len(expected.RegionList) != len(actual.RegionList):
        errors.append(
            f"RegionInfo.RegionList length mismatch: expected {len(expected.RegionList)}, got {len(actual.RegionList)}",
        )
        return errors

    for i, (exp_region, act_region) in enumerate(zip(expected.RegionList, actual.RegionList)):
        # Compare Area
        exp_area = exp_region.Area
        act_area = act_region.Area

        if exp_area.H != act_area.H:
            errors.append(f"RegionList[{i}].Area.H mismatch: expected {exp_area.H}, got {act_area.H}")
        if exp_area.W != act_area.W:
            errors.append(f"RegionList[{i}].Area.W mismatch: expected {exp_area.W}, got {act_area.W}")
        if exp_area.X != act_area.X:
            errors.append(f"RegionList[{i}].Area.X mismatch: expected {exp_area.X}, got {act_area.X}")
        if exp_area.Y != act_area.Y:
            errors.append(f"RegionList[{i}].Area.Y mismatch: expected {exp_area.Y}, got {act_area.Y}")
        if exp_area.Unit != act_area.Unit:
            errors.append(f"RegionList[{i}].Area.Unit mismatch: expected '{exp_area.Unit}', got '{act_area.Unit}'")
        if exp_area.D != act_area.D:
            errors.append(f"RegionList[{i}].Area.D mismatch: expected {exp_area.D}, got {act_area.D}")

        # Compare other region fields
        if exp_region.Name != act_region.Name:
            errors.append(f"RegionList[{i}].Name mismatch: expected '{exp_region.Name}', got '{act_region.Name}'")
        if exp_region.Type != act_region.Type:
            errors.append(f"RegionList[{i}].Type mismatch: expected '{exp_region.Type}', got '{act_region.Type}'")
        if exp_region.Description != act_region.Description:
            errors.append(
                f"RegionList[{i}].Description mismatch: expected '{exp_region.Description}', got '{act_region.Description}'",
            )

    return errors


def compare_keyword_info(expected: Optional[KeywordInfoModel], actual: Optional[KeywordInfoModel]):
    """Compare KeywordInfo structures and return detailed error messages."""
    errors = []

    if expected is None and actual is None:
        return errors
    if expected is None or actual is None:
        errors.append(f"KeywordInfo mismatch: expected {expected}, got {actual}")
        return errors

    def compare_keyword_struct(exp_kw, act_kw, path=""):
        kw_errors = []
        if exp_kw.Keyword != act_kw.Keyword:
            kw_errors.append(f"KeywordInfo{path}.Keyword mismatch: expected '{exp_kw.Keyword}', got '{act_kw.Keyword}'")
        if exp_kw.Applied != act_kw.Applied:
            kw_errors.append(f"KeywordInfo{path}.Applied mismatch: expected {exp_kw.Applied}, got {act_kw.Applied}")
        if len(exp_kw.Children) != len(act_kw.Children):
            kw_errors.append(
                f"KeywordInfo{path}.Children length mismatch: expected {len(exp_kw.Children)}, got {len(act_kw.Children)}",
            )
        else:
            for i, (exp_child, act_child) in enumerate(zip(exp_kw.Children, act_kw.Children)):
                kw_errors.extend(compare_keyword_struct(exp_child, act_child, f"{path}.Children[{i}]"))
        return kw_errors

    if len(expected.Hierarchy) != len(actual.Hierarchy):
        errors.append(
            f"KeywordInfo.Hierarchy length mismatch: expected {len(expected.Hierarchy)}, got {len(actual.Hierarchy)}",
        )
    else:
        for i, (exp_kw, act_kw) in enumerate(zip(expected.Hierarchy, actual.Hierarchy)):
            errors.extend(compare_keyword_struct(exp_kw, act_kw, f".Hierarchy[{i}]"))

    return errors


def verify_expected_vs_actual_metadata(expected: ImageMetadata, actual: ImageMetadata):
    """
    Compare two ImageMetadata instances and fail with detailed error messages.

    Args:
        expected: The expected ImageMetadata instance
        actual: The actual ImageMetadata instance

    Raises:
        pytest.fail with detailed error message if any fields don't match
    """
    errors = []

    # Basic fields
    if expected.SourceFile != actual.SourceFile:
        errors.append(f"SourceFile mismatch: expected {expected.SourceFile}, got {actual.SourceFile}")

    if expected.ImageHeight != actual.ImageHeight:
        errors.append(f"ImageHeight mismatch: expected {expected.ImageHeight}, got {actual.ImageHeight}")

    if expected.ImageWidth != actual.ImageWidth:
        errors.append(f"ImageWidth mismatch: expected {expected.ImageWidth}, got {actual.ImageWidth}")

    # Optional string fields
    error = compare_optional_values("Title", expected.Title, actual.Title)
    if error:
        errors.append(error)

    error = compare_optional_values("Description", expected.Description, actual.Description)
    if error:
        errors.append(error)

    error = compare_optional_values("Orientation", expected.Orientation, actual.Orientation)
    if error:
        errors.append(error)

    error = compare_optional_values("Country", expected.Country, actual.Country)
    if error:
        errors.append(error)

    error = compare_optional_values("City", expected.City, actual.City)
    if error:
        errors.append(error)

    error = compare_optional_values("State", expected.State, actual.State)
    if error:
        errors.append(error)

    error = compare_optional_values("Location", expected.Location, actual.Location)
    if error:
        errors.append(error)

    # List fields
    error = compare_lists("LastKeywordXMP", expected.LastKeywordXMP, actual.LastKeywordXMP)
    if error:
        errors.append(error)

    error = compare_lists("TagsList", expected.TagsList, actual.TagsList)
    if error:
        errors.append(error)

    error = compare_lists("CatalogSets", expected.CatalogSets, actual.CatalogSets)
    if error:
        errors.append(error)

    error = compare_lists("HierarchicalSubject", expected.HierarchicalSubject, actual.HierarchicalSubject)
    if error:
        errors.append(error)

    # Complex structures
    errors.extend(compare_region_info(expected.RegionInfo, actual.RegionInfo))
    errors.extend(compare_keyword_info(expected.KeywordInfo, actual.KeywordInfo))

    # If any errors found, fail with detailed message
    if errors:
        error_message = "ImageMetadata comparison failed:\n" + "\n".join(f"  - {error}" for error in errors)
        pytest.fail(error_message)
