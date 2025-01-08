import logging
import os
import random
import shutil
from collections.abc import Generator
from pathlib import Path

import pytest

from exifmwg import ExifTool
from exifmwg.models import ImageMetadata


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


@pytest.fixture
def exiftool(exiftool_path: Path, logger: logging.Logger) -> Generator[ExifTool, None, None]:
    with ExifTool(executable=str(exiftool_path)) as tool:
        tool.logger = logger
        yield tool


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


@pytest.fixture(scope="session")
def sample_one_metadata_original(fixture_directory: Path) -> ImageMetadata:
    return ImageMetadata.model_validate_json((fixture_directory / "sample1.jpg.json").read_text())


@pytest.fixture
def sample_one_metadata_copy(
    tmp_path: Path,
    sample_one_original_file: Path,
    sample_one_metadata_original: ImageMetadata,
) -> ImageMetadata:
    cpy = sample_one_metadata_original.model_copy(deep=True)
    cpy.SourceFile = shutil.copy(sample_one_original_file, tmp_path / sample_one_original_file.name)
    return cpy


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
