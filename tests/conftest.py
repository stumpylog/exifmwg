import logging
import os
import random
import shutil
from pathlib import Path

import pytest

from exifmwg import DimensionsStruct
from exifmwg import ImageMetadata
from exifmwg import KeywordInfoModel
from exifmwg import KeywordStruct
from exifmwg import RegionInfoStruct
from exifmwg import RegionStruct
from exifmwg import XmpAreaStruct


@pytest.fixture(scope="session", autouse=True)
def faker_seed():
    return 0


@pytest.fixture(scope="session", autouse=True)
def _seed_random():
    random.seed(0)


@pytest.fixture
def logger(request):
    # Create logger
    logger = logging.getLogger(f"exifmwg.test.{request.node.name}")
    logger.setLevel(logging.DEBUG)

    # Create console handler and set level
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)

    # Create formatter
    formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
    ch.setFormatter(formatter)

    # Add handler to logger
    logger.addHandler(ch)

    yield logger

    # Cleanup after test
    logger.handlers.clear()


@pytest.fixture(scope="session")
def exiftool_path() -> Path:
    exiftool = shutil.which("exiftool")
    if exiftool is not None:
        return Path(exiftool).resolve()
    exiftool = os.environ.get("EXIFTOOL_PATH")
    if exiftool is not None:
        return Path(exiftool).resolve()
    raise pytest.UsageError("Unable to locate exiftool executable via PATH or environment")  # noqa: EM101, TRY003


#
# Base Directories
#


@pytest.fixture(scope="session")
def fixture_directory() -> Path:
    return Path(__file__).parent / "fixtures"


@pytest.fixture(scope="session")
def sample_directory() -> Path:
    return Path(__file__).parent / "samples"


#
# Sample Directories
#


@pytest.fixture(scope="session")
def image_sample_directory(sample_directory: Path) -> Path:
    return sample_directory / "images"


#
# Sample 1
#


@pytest.fixture(scope="session")
def sample_one_original_file(image_sample_directory: Path) -> Path:
    return image_sample_directory / "sample1.jpg"


@pytest.fixture
def sample_one_original_copy(tmp_path: Path, sample_one_original_file: Path) -> Path:
    return Path(shutil.copy(sample_one_original_file, tmp_path))


@pytest.fixture
def sample_one_metadata_original(fixture_directory: Path) -> ImageMetadata:
    return ImageMetadata(
        SourceFile=Path("tests/samples/images/sample1.jpg"),
        ImageHeight=683,
        ImageWidth=1024,
        Title=None,
        Description="President Barack Obama throws a ball for Bo, the family dog, in the Rose Garden of the White House, Sept. 9, 2010.  (Official White House Photo by Pete Souza)",
        RegionInfo=RegionInfoStruct(
            AppliedToDimensions=DimensionsStruct(H=683.0, W=1024.0, Unit="pixel"),
            RegionList=[
                RegionStruct(
                    Area=XmpAreaStruct(H=0.0585652, Unit="normalized", W=0.0292969, X=0.317383, Y=0.303075, D=None),
                    Name="Barack Obama",
                    Type="Face",
                    Description=None,
                ),
                RegionStruct(
                    Area=XmpAreaStruct(H=0.284041, Unit="normalized", W=0.202148, X=0.616699, Y=0.768668, D=None),
                    Name="Bo",
                    Type="Pet",
                    Description="Bo was a pet dog of the Obama family",
                ),
            ],
        ),
        Orientation=None,
        LastKeywordXMP=[
            "People/Barack Obama",
            "Locations/United States/District of Columbia/Washington DC",
            "Dates/2010/09 - September/9",
            "Pets/Dogs/Bo",
        ],
        TagsList=[
            "People/Barack Obama",
            "Locations/United States/District of Columbia/Washington DC",
            "Dates/2010/09 - September/9",
            "Pets/Dogs/Bo",
        ],
        CatalogSets=[
            "People|Barack Obama",
            "Locations|United States|District of Columbia|Washington DC",
            "Dates|2010|09 - September|9",
            "Pets|Dogs|Bo",
        ],
        HierarchicalSubject=[
            "People|Barack Obama",
            "Locations|United States|District of Columbia|Washington DC",
            "Dates|2010|09 - September|9",
            "Pets|Dogs|Bo",
        ],
        KeywordInfo=KeywordInfoModel(
            Hierarchy=[
                KeywordStruct(
                    Keyword="People",
                    Applied=None,
                    Children=[KeywordStruct(Keyword="Barack Obama", Applied=None, Children=[])],
                ),
                KeywordStruct(
                    Keyword="Locations",
                    Applied=None,
                    Children=[
                        KeywordStruct(
                            Keyword="United States",
                            Applied=None,
                            Children=[
                                KeywordStruct(
                                    Keyword="District of Columbia",
                                    Applied=None,
                                    Children=[KeywordStruct(Keyword="Washington DC", Applied=None, Children=[])],
                                ),
                            ],
                        ),
                    ],
                ),
                KeywordStruct(
                    Keyword="Dates",
                    Applied=None,
                    Children=[
                        KeywordStruct(
                            Keyword="2010",
                            Applied=None,
                            Children=[
                                KeywordStruct(
                                    Keyword="09 - September",
                                    Applied=None,
                                    Children=[KeywordStruct(Keyword="9", Applied=None, Children=[])],
                                ),
                            ],
                        ),
                    ],
                ),
                KeywordStruct(
                    Keyword="Pets",
                    Applied=None,
                    Children=[
                        KeywordStruct(
                            Keyword="Dogs",
                            Applied=None,
                            Children=[KeywordStruct(Keyword="Bo", Applied=None, Children=[])],
                        ),
                    ],
                ),
            ],
        ),
        Country="USA",
        City="WASHINGTON",
        State="DC",
        Location=None,
    )


@pytest.fixture
def sample_one_metadata_copy(
    tmp_path: Path,
    sample_one_original_file: Path,
    sample_one_metadata_original: ImageMetadata,
) -> ImageMetadata:
    sample_one_metadata_original.SourceFile = shutil.copy(
        sample_one_original_file,
        tmp_path / sample_one_original_file.name,
    )
    return sample_one_metadata_original


#
# Sample 2
#


@pytest.fixture(scope="session")
def sample_two_original_file(image_sample_directory: Path) -> Path:
    return image_sample_directory / "sample2.jpg"


@pytest.fixture
def sample_two_original_copy(tmp_path: Path, sample_two_original_file: Path) -> Path:
    return Path(shutil.copy(sample_two_original_file, tmp_path))


@pytest.fixture(scope="session")
def sample_two_metadata_original(fixture_directory: Path) -> ImageMetadata:
    return ImageMetadata.model_validate_json((fixture_directory / "sample2.jpg.json").read_text())


@pytest.fixture
def sample_two_metadata_copy(
    tmp_path: Path,
    sample_two_original_file: Path,
    sample_two_metadata_original: ImageMetadata,
) -> ImageMetadata:
    cpy = sample_two_metadata_original.model_copy(deep=True)
    cpy.SourceFile = shutil.copy(sample_two_original_file, tmp_path / sample_two_original_file.name)
    return cpy


#
# Sample 3
#


@pytest.fixture(scope="session")
def sample_three_original_file(image_sample_directory: Path) -> Path:
    return image_sample_directory / "sample3.jpg"


@pytest.fixture
def sample_three_original_copy(tmp_path: Path, sample_three_original_file: Path) -> Path:
    return Path(shutil.copy(sample_three_original_file, tmp_path))


@pytest.fixture(scope="session")
def sample_three_metadata_original(fixture_directory: Path) -> ImageMetadata:
    return ImageMetadata.model_validate_json((fixture_directory / "sample3.jpg.json").read_text())


@pytest.fixture
def sample_three_metadata_copy(
    tmp_path: Path,
    sample_three_original_file: Path,
    sample_three_metadata_original: ImageMetadata,
) -> ImageMetadata:
    cpy = sample_three_metadata_original.model_copy(deep=True)
    cpy.SourceFile = shutil.copy(sample_three_original_file, tmp_path / sample_three_original_file.name)
    return cpy


#
# Sample 4
#


@pytest.fixture(scope="session")
def sample_four_original_file(image_sample_directory: Path) -> Path:
    return image_sample_directory / "sample4.jpg"


@pytest.fixture
def sample_four_original_copy(tmp_path: Path, sample_four_original_file: Path) -> Path:
    return Path(shutil.copy(sample_four_original_file, tmp_path))


@pytest.fixture(scope="session")
def sample_four_metadata_original(fixture_directory: Path) -> ImageMetadata:
    return ImageMetadata.model_validate_json((fixture_directory / "sample4.jpg.json").read_text())


@pytest.fixture
def sample_four_metadata_copy(
    tmp_path: Path,
    sample_four_original_file: Path,
    sample_four_metadata_original: ImageMetadata,
) -> ImageMetadata:
    cpy = sample_four_metadata_original.model_copy(deep=True)
    cpy.SourceFile = shutil.copy(sample_four_original_file, tmp_path / sample_four_original_file.name)
    return cpy
