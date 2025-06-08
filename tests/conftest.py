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
def sample_one_metadata_original(sample_one_original_file: Path) -> ImageMetadata:
    return ImageMetadata(
        SourceFile=sample_one_original_file,
        ImageHeight=683,
        ImageWidth=1024,
        Title=None,
        Description="President Barack Obama throws a ball for Bo, the family dog, in the Rose Garden of the White "
        "House, Sept. 9, 2010.  (Official White House Photo by Pete Souza)",
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
    sample_one_original_copy: Path,
    sample_one_metadata_original: ImageMetadata,
) -> ImageMetadata:
    sample_one_metadata_original.SourceFile = sample_one_original_copy
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
def sample_two_metadata_original(sample_two_original_file: Path) -> ImageMetadata:
    return ImageMetadata(
        SourceFile=sample_two_original_file,
        ImageHeight=2333,
        ImageWidth=3500,
        Title=None,
        Description="President Barack Obama signs a letter to a Cuban letter writer, in the Oval Office, "
        "March 14, 2016. (Official White House Photo by Pete Souza)\n\nThis official White House photograph is being "
        "made available only for publication by news organizations and/or for personal use printing by the subject(s) "
        "of the photograph. The photograph may not be manipulated in any way and may not be used in commercial or "
        "political materials, advertisements, emails, products, promotions that in any way suggests approval or "
        "endorsement of the President, the First Family, or the White House.",
        RegionInfo=RegionInfoStruct(
            AppliedToDimensions=DimensionsStruct(H=2333.0, W=3500.0, Unit="pixel"),
            RegionList=[
                RegionStruct(
                    Area=XmpAreaStruct(H=0.216459, Unit="normalized", W=0.129714, X=0.492857, Y=0.277968, D=None),
                    Name="Barack Obama",
                    Type="Face",
                    Description=None,
                ),
            ],
        ),
        Orientation=1,
        LastKeywordXMP=["Locations/United States/Washington DC", "People/Barack Obama"],
        TagsList=["Locations/United States/Washington DC", "People/Barack Obama"],
        CatalogSets=["Locations|United States|Washington DC", "People|Barack Obama"],
        HierarchicalSubject=[
            "Locations|United States|Washington DC",
            "People|Barack Obama",
        ],
        KeywordInfo=KeywordInfoModel(
            Hierarchy=[
                KeywordStruct(
                    Keyword="Locations",
                    Applied=None,
                    Children=[
                        KeywordStruct(
                            Keyword="United States",
                            Applied=None,
                            Children=[
                                KeywordStruct(Keyword="Washington DC", Applied=None, Children=[]),
                            ],
                        ),
                    ],
                ),
                KeywordStruct(
                    Keyword="People",
                    Applied=None,
                    Children=[KeywordStruct(Keyword="Barack Obama", Applied=None, Children=[])],
                ),
            ],
        ),
        Country="USA",
        City="WASHINGTON",
        State="DC",
        Location=None,
    )


@pytest.fixture
def sample_two_metadata_copy(
    sample_two_original_copy: Path,
    sample_two_metadata_original: ImageMetadata,
) -> ImageMetadata:
    sample_two_metadata_original.SourceFile = sample_two_original_copy
    return sample_two_metadata_original


#
# Sample 3
#


@pytest.fixture(scope="session")
def sample_three_original_file(image_sample_directory: Path) -> Path:
    return image_sample_directory / "sample3.jpg"


@pytest.fixture
def sample_three_original_copy(tmp_path: Path, sample_three_original_file: Path) -> Path:
    return Path(shutil.copy(sample_three_original_file, tmp_path))


@pytest.fixture
def sample_three_metadata_original(sample_three_original_file: Path) -> ImageMetadata:
    return ImageMetadata(
        SourceFile=sample_three_original_file,
        ImageHeight=1000,
        ImageWidth=1500,
        Title=None,
        Description='May 1, 2011\n"Much has been made of this photograph that shows the President and Vice President '
        "and the national security team monitoring in real time the mission against Osama bin Laden. Some more "
        "background on the photograph: The White House Situation Room is actually comprised of several different "
        "conference rooms. The majority of the time, the President convenes meetings in the large conference room with "
        "assigned seats. But to monitor this mission, the group moved into the much smaller conference room. The "
        'President chose to sit next to Brigadier General Marshall B. "Brad" Webb, Assistant Commanding General of '
        "Joint Special Operations Command, who was point man for the communications taking place. WIth so few chairs, "
        "others just stood at the back of the room. I was jammed into a corner of the room with no room to move. "
        "During the mission itself, I made approximately 100 photographs, almost all from this cramped spot in the "
        "corner. There were several other meetings throughout the day, and we've put together a composite of several "
        "photographs (see next photo in this set) to give people a better sense of what the day was like.  Seated in "
        "this picture from left to right: Vice President Biden, the President, Brig. Gen. Webb, Deputy National "
        "Security Advisor Denis McDonough, Secretary of State Hillary Rodham Clinton, and then Secretary of Defense "
        "Robert Gates. Standing, from left, are: Admiral Mike Mullen, then Chairman of the Joint Chiefs of Staff; "
        "National Security Advisor Tom Donilon; Chief of Staff Bill Daley; Tony Blinken, National Security Advisor "
        "to the Vice President; Audrey Tomason Director for Counterterrorism; John Brennan, Assistant to the "
        "President for Homeland Security and Counterterrorism; and Director of National Intelligence James Clapper. "
        'Please note: a classified document seen in front of Sec. Clinton has been obscured." \n(Official White House '
        "Photo by Pete Souza)\n\nThis official White House photograph is being made available only for publication by "
        "news organizations and/or for personal use printing by the subject(s) of the photograph. The photograph may "
        "not be manipulated in any way and may not be used in commercial or political materials, advertisements, "
        "emails, products, promotions that in any way suggests approval or endorsement of the President, the First "
        "Family, or the White House.",
        RegionInfo=RegionInfoStruct(
            AppliedToDimensions=DimensionsStruct(H=1000.0, W=1500.0, Unit="pixel"),
            RegionList=[
                RegionStruct(
                    Area=XmpAreaStruct(H=0.072, Unit="normalized", W=0.0386667, X=0.332667, Y=0.39, D=None),
                    Name="Barack Obama",
                    Type="Face",
                    Description=None,
                ),
                RegionStruct(
                    Area=XmpAreaStruct(H=0.079, Unit="normalized", W=0.0386667, X=0.489333, Y=0.1445, D=None),
                    Name="Denis McDonough",
                    Type="Face",
                    Description=None,
                ),
                RegionStruct(
                    Area=XmpAreaStruct(H=0.108, Unit="normalized", W=0.0626667, X=0.780667, Y=0.484, D=None),
                    Name="Hillary Clinton",
                    Type="Face",
                    Description=None,
                ),
                RegionStruct(
                    Area=XmpAreaStruct(H=0.094, Unit="normalized", W=0.0433333, X=0.0916667, Y=0.419, D=None),
                    Name="Joseph R Biden",
                    Type="Face",
                    Description=None,
                ),
            ],
        ),
        Orientation=1,
        LastKeywordXMP=[
            "Locations/United States/Washington DC",
            "People/Barack Obama",
            "People/Denis McDonough",
            "People/Hillary Clinton",
            "People/Joseph R Biden",
        ],
        TagsList=[
            "Locations/United States/Washington DC",
            "People/Barack Obama",
            "People/Denis McDonough",
            "People/Hillary Clinton",
            "People/Joseph R Biden",
        ],
        CatalogSets=[
            "Locations|United States|Washington DC",
            "People|Barack Obama",
            "People|Denis McDonough",
            "People|Hillary Clinton",
            "People|Joseph R Biden",
        ],
        HierarchicalSubject=[
            "Locations|United States|Washington DC",
            "People|Barack Obama",
            "People|Denis McDonough",
            "People|Hillary Clinton",
            "People|Joseph R Biden",
        ],
        KeywordInfo=KeywordInfoModel(
            Hierarchy=[
                KeywordStruct(
                    Keyword="Locations",
                    Applied=None,
                    Children=[
                        KeywordStruct(
                            Keyword="United States",
                            Applied=None,
                            Children=[
                                KeywordStruct(Keyword="Washington DC", Applied=None, Children=[]),
                            ],
                        ),
                    ],
                ),
                KeywordStruct(
                    Keyword="People",
                    Applied=None,
                    Children=[
                        KeywordStruct(Keyword="Joseph R Biden", Applied=None, Children=[]),
                        KeywordStruct(Keyword="Denis McDonough", Applied=None, Children=[]),
                        KeywordStruct(Keyword="Hillary Clinton", Applied=None, Children=[]),
                        KeywordStruct(Keyword="Barack Obama", Applied=None, Children=[]),
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
def sample_three_metadata_copy(
    sample_three_original_copy: Path,
    sample_three_metadata_original: ImageMetadata,
) -> ImageMetadata:
    sample_three_metadata_original.SourceFile = sample_three_original_copy
    return sample_three_metadata_original


#
# Sample 4
#


@pytest.fixture(scope="session")
def sample_four_original_file(image_sample_directory: Path) -> Path:
    return image_sample_directory / "sample4.jpg"


@pytest.fixture
def sample_four_original_copy(tmp_path: Path, sample_four_original_file: Path) -> Path:
    return Path(shutil.copy(sample_four_original_file, tmp_path))


@pytest.fixture
def sample_four_metadata_original(sample_four_original_file: Path) -> ImageMetadata:
    return ImageMetadata(
        SourceFile=sample_four_original_file,
        ImageHeight=436,
        ImageWidth=654,
        Title=None,
        Description="CREATOR: gd-jpeg v1.0 (using IJG JPEG v62), quality = 100\n",
        RegionInfo=RegionInfoStruct(
            AppliedToDimensions=DimensionsStruct(H=436.0, W=654.0, Unit="pixel"),
            RegionList=[
                RegionStruct(
                    Area=XmpAreaStruct(H=0.0940367, Unit="normalized", W=0.0428135, X=0.466361, Y=0.186927, D=None),
                    Name="Barack Obama",
                    Type="Face",
                    Description=None,
                ),
            ],
        ),
        Orientation=None,
        LastKeywordXMP=[
            "Locations/United States/Washington DC",
            "Pets/Dogs/Bo",
            "People/Barack Obama",
        ],
        TagsList=[
            "Locations/United States/Washington DC",
            "Pets/Dogs/Bo",
            "People/Barack Obama",
        ],
        CatalogSets=[
            "Locations|United States|Washington DC",
            "Pets|Dogs|Bo",
            "People|Barack Obama",
        ],
        HierarchicalSubject=[
            "Locations|United States|Washington DC",
            "Pets|Dogs|Bo",
            "People|Barack Obama",
        ],
        KeywordInfo=KeywordInfoModel(
            Hierarchy=[
                KeywordStruct(
                    Keyword="Locations",
                    Applied=None,
                    Children=[
                        KeywordStruct(
                            Keyword="United States",
                            Applied=None,
                            Children=[
                                KeywordStruct(Keyword="Washington DC", Applied=None, Children=[]),
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
                KeywordStruct(
                    Keyword="People",
                    Applied=None,
                    Children=[KeywordStruct(Keyword="Barack Obama", Applied=None, Children=[])],
                ),
            ],
        ),
        Country=None,
        City=None,
        State=None,
        Location=None,
    )


@pytest.fixture
def sample_four_metadata_copy(
    sample_four_original_copy: Path,
    sample_four_metadata_original: ImageMetadata,
) -> ImageMetadata:
    sample_four_metadata_original.SourceFile = sample_four_original_copy
    return sample_four_metadata_original
