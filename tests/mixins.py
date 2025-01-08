from __future__ import annotations

import collections
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from exif_mwg.models import ImageMetadata


class MetadataVerifyMixin:
    """
    Utilities for verifying sample image metadata and metadata in general against another version
    """

    def assert_count_equal(self, expected: list[str | int] | None, actual: list[str | int] | None) -> None:
        """
        https://github.com/python/cpython/blob/17d31bf3843c384873999a15ce683cc3654f46ae/Lib/unittest/case.py#L1186
        """
        expected_seq, actual_seq = list(expected or []), list(actual or [])
        expected_counter = collections.Counter(expected_seq)
        actual_counter = collections.Counter(actual_seq)
        assert expected_counter == actual_counter

    def verify_expected_vs_actual_metadata(self, expected: ImageMetadata, actual: ImageMetadata):
        assert expected.SourceFile == actual.SourceFile
        assert expected.Title == actual.Title
        assert expected.Description == actual.Description

        if expected.RegionInfo is None or actual.RegionInfo is None:
            assert expected.RegionInfo == actual.RegionInfo
        else:
            assert expected.RegionInfo.model_dump() == actual.RegionInfo.model_dump()

        assert expected.Orientation == actual.Orientation
        self.assert_count_equal(expected.LastKeywordXMP, actual.LastKeywordXMP)
        self.assert_count_equal(expected.TagsList, actual.TagsList)
        self.assert_count_equal(expected.CatalogSets, actual.CatalogSets)
        self.assert_count_equal(expected.HierarchicalSubject, actual.HierarchicalSubject)
        if expected.KeywordInfo is None or actual.KeywordInfo is None:
            assert expected.KeywordInfo == actual.KeywordInfo
        else:
            assert expected.KeywordInfo.model_dump() == actual.KeywordInfo.model_dump()
