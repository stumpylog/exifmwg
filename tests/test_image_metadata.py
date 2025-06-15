from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from exifmwg import Dimensions
from exifmwg import ImageMetadata
from exifmwg import Keyword
from exifmwg import KeywordInfo
from exifmwg import Region
from exifmwg import RegionInfo
from exifmwg import XmpArea
from exifmwg import exiv2_version
from exifmwg import expat_version
from exifmwg import read_metadata
from exifmwg import write_metadata
from tests.utils import verify_image_metadata
from tests.utils import verify_keyword_info

if TYPE_CHECKING:
    from pathlib import Path


class TestReadImageMetadata:
    @pytest.mark.parametrize(
        ("original_file_fixture_name", "expected_metadata_fixture_name"),
        [
            pytest.param("sample_one_original_file", "sample_one_metadata", id="image_one"),
            pytest.param("sample_two_original_file", "sample_two_metadata", id="image_two"),
            pytest.param("sample_three_original_file", "sample_three_metadata", id="image_three"),
            pytest.param("sample_four_original_file", "sample_four_metadata", id="image_four"),
        ],
    )
    def test_read_image_metadata(
        self,
        original_file_fixture_name: str,
        expected_metadata_fixture_name: str,
        request: pytest.FixtureRequest,
    ) -> None:
        original_file: Path = request.getfixturevalue(original_file_fixture_name)
        expected_metadata: ImageMetadata = request.getfixturevalue(expected_metadata_fixture_name)

        metadata = read_metadata(original_file)

        verify_image_metadata(expected_metadata, metadata)


class TestKeywordInfoConstructions:
    @pytest.mark.parametrize(
        ("input_list", "delimiter"),
        [
            pytest.param(
                ["Locations/United States/Washington DC", "People/Barack Obama"],
                "/",
                id="with_slash_delim",
            ),
            pytest.param(
                ["Locations/United States/Washington DC", "People/Barack Obama"],
                None,
                id="with_no_set_delim",
            ),
            pytest.param(
                ["Locations|United States|Washington DC", "People|Barack Obama"],
                "|",
                id="with_pipe_delim",
            ),
        ],
    )
    def test_keyword_info_construction(self, input_list: list[str], delimiter: str | None):
        expected = KeywordInfo(
            hierarchy=[
                Keyword(
                    keyword="Locations",
                    applied=None,
                    children=[
                        Keyword(
                            keyword="United States",
                            applied=None,
                            children=[Keyword(keyword="Washington DC", applied=None, children=[])],
                        ),
                    ],
                ),
                Keyword(
                    keyword="People",
                    applied=None,
                    children=[Keyword(keyword="Barack Obama", applied=None, children=[])],
                ),
            ],
        )

        actual = KeywordInfo(input_list, delimiter) if delimiter is not None else KeywordInfo(input_list)

        verify_keyword_info(expected, actual)


class TestWriteImageMetadata:
    def test_change_single_image_metadata(self, sample_one_image_copy: Path, sample_one_metadata: ImageMetadata):
        sample_one_metadata.title = "This is a new title"
        sample_one_metadata.description = "This is a new description"
        sample_one_metadata.orientation = 5
        sample_one_metadata.country = "CA"
        sample_one_metadata.state = "CA-BC"
        sample_one_metadata.city = "Vancouver"
        sample_one_metadata.location = "Science World"
        sample_one_metadata.last_keyword_xmp = ["LXK Root 1", "LXK Root 2/Root 2 Child 1"]
        sample_one_metadata.tags_list = ["TGLST Root 1", "TGLST Root 2/Root 2 Child 1"]
        sample_one_metadata.catalog_sets = ["CTS Root 1", "CTS Root 2|Root 2 Child 1"]
        sample_one_metadata.hierarchical_subject = ["HRCY Root 1", "HRCY Root 2|Root 2 Child 1"]
        sample_one_metadata.keyword_info = KeywordInfo(
            hierarchy=[
                Keyword(
                    keyword="People",
                    applied=None,
                    children=[Keyword(keyword="Barack Obama", applied=None, children=[])],
                ),
            ],
        )
        sample_one_metadata.region_info = RegionInfo(
            applied_to_dimensions=Dimensions(h=683.0, w=1024.0, unit="pixel"),
            region_list=[
                Region(
                    XmpArea(h=0.1, w=0.1, x=0.1, y=0.1, unit="normalized"),
                    "Sally Sue",
                    "Face",
                    "This is Aunt Sally Sue",
                ),
                Region(
                    XmpArea(h=0.2, w=0.3, x=0.4, y=0.5, unit="normalized"),
                    "Billy Bob",
                    "Face",
                    "And her husband Billy Bob",
                ),
                Region(XmpArea(h=0.6, w=0.7, x=0.8, y=0.9, unit="normalized", d=1.0), "Little John Bob", "Face"),
            ],
        )

        write_metadata(sample_one_image_copy, sample_one_metadata)

        changed_metadata = read_metadata(sample_one_image_copy)

        verify_image_metadata(sample_one_metadata, changed_metadata)


class TestMetadataClear:
    def test_clear_existing_metadata(self):
        pass


class TestErrorCases:
    pass


class TestVersionInfo:
    def test_exiv2_version(self):
        assert exiv2_version() == "0.28.5"

    def test_expat_version(self):
        assert expat_version() == "expact_2.7.1"
