import logging
import random
import shutil
from pathlib import Path

import pytest

# Updated imports to use the new Pythonic class names
from exifmwg import Dimensions
from exifmwg import ImageMetadata
from exifmwg import Keyword
from exifmwg import KeywordInfo
from exifmwg import Region
from exifmwg import RegionInfo
from exifmwg import XmpArea


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
    # Changed attribute names to snake_case
    return ImageMetadata(
        source_file=sample_one_original_file,
        image_height=683,
        image_width=1024,
        title=None,
        description="President Barack Obama throws a ball for Bo, the family dog, in the Rose Garden of the White "
        "House, Sept. 9, 2010.  (Official White House Photo by Pete Souza)",
        region_info=RegionInfo(
            applied_to_dimensions=Dimensions(h=683.0, w=1024.0, unit="pixel"),
            region_list=[
                Region(
                    area=XmpArea(h=0.0585652, unit="normalized", w=0.0292969, x=0.317383, y=0.303075, d=None),
                    name="Barack Obama",
                    type_="Face",
                    description=None,
                ),
                Region(
                    area=XmpArea(h=0.284041, unit="normalized", w=0.202148, x=0.616699, y=0.768668, d=None),
                    name="Bo",
                    type_="Pet",
                    description="Bo was a pet dog of the Obama family",
                ),
            ],
        ),
        orientation=None,
        last_keyword_xmp=[
            "People/Barack Obama",
            "Locations/United States/District of Columbia/Washington DC",
            "Dates/2010/09 - September/9",
            "Pets/Dogs/Bo",
        ],
        tags_list=[
            "People/Barack Obama",
            "Locations/United States/District of Columbia/Washington DC",
            "Dates/2010/09 - September/9",
            "Pets/Dogs/Bo",
        ],
        catalog_sets=[
            "People|Barack Obama",
            "Locations|United States|District of Columbia|Washington DC",
            "Dates|2010|09 - September|9",
            "Pets|Dogs|Bo",
        ],
        hierarchical_subject=[
            "People|Barack Obama",
            "Locations|United States|District of Columbia|Washington DC",
            "Dates|2010|09 - September|9",
            "Pets|Dogs|Bo",
        ],
        keyword_info=KeywordInfo(
            hierarchy=[
                Keyword(
                    keyword="People",
                    applied=None,
                    children=[Keyword(keyword="Barack Obama", applied=None, children=[])],
                ),
                Keyword(
                    keyword="Locations",
                    applied=None,
                    children=[
                        Keyword(
                            keyword="United States",
                            applied=None,
                            children=[
                                Keyword(
                                    keyword="District of Columbia",
                                    applied=None,
                                    children=[Keyword(keyword="Washington DC", applied=None, children=[])],
                                ),
                            ],
                        ),
                    ],
                ),
                Keyword(
                    keyword="Dates",
                    applied=None,
                    children=[
                        Keyword(
                            keyword="2010",
                            applied=None,
                            children=[
                                Keyword(
                                    keyword="09 - September",
                                    applied=None,
                                    children=[Keyword(keyword="9", applied=None, children=[])],
                                ),
                            ],
                        ),
                    ],
                ),
                Keyword(
                    keyword="Pets",
                    applied=None,
                    children=[
                        Keyword(
                            keyword="Dogs",
                            applied=None,
                            children=[Keyword(keyword="Bo", applied=None, children=[])],
                        ),
                    ],
                ),
            ],
        ),
        country="USA",
        city="WASHINGTON",
        state="DC",
        location=None,
    )


@pytest.fixture
def sample_one_metadata_copy(
    sample_one_original_copy: Path,
    sample_one_metadata_original: ImageMetadata,
) -> ImageMetadata:
    # Changed attribute access to snake_case
    sample_one_metadata_original.source_file = sample_one_original_copy
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
    # Changed attribute names to snake_case
    return ImageMetadata(
        source_file=sample_two_original_file,
        image_height=2333,
        image_width=3500,
        title=None,
        description="President Barack Obama signs a letter to a Cuban letter writer, in the Oval Office, "
        "March 14, 2016. (Official White House Photo by Pete Souza)\n\nThis official White House photograph is being "
        "made available only for publication by news organizations and/or for personal use printing by the subject(s) "
        "of the photograph. The photograph may not be manipulated in any way and may not be used in commercial or "
        "political materials, advertisements, emails, products, promotions that in any way suggests approval or "
        "endorsement of the President, the First Family, or the White House.",
        region_info=RegionInfo(
            applied_to_dimensions=Dimensions(h=2333.0, w=3500.0, unit="pixel"),
            region_list=[
                Region(
                    area=XmpArea(h=0.216459, unit="normalized", w=0.129714, x=0.492857, y=0.277968, d=None),
                    name="Barack Obama",
                    type_="Face",
                    description=None,
                ),
            ],
        ),
        orientation=1,
        last_keyword_xmp=["Locations/United States/Washington DC", "People/Barack Obama"],
        tags_list=["Locations/United States/Washington DC", "People/Barack Obama"],
        catalog_sets=["Locations|United States|Washington DC", "People|Barack Obama"],
        hierarchical_subject=[
            "Locations|United States|Washington DC",
            "People|Barack Obama",
        ],
        keyword_info=KeywordInfo(hierarchy=[]),
        country="USA",
        city="WASHINGTON",
        state="DC",
        location=None,
    )


@pytest.fixture
def sample_two_metadata_copy(
    sample_two_original_copy: Path,
    sample_two_metadata_original: ImageMetadata,
) -> ImageMetadata:
    # Changed attribute access to snake_case
    sample_two_metadata_original.source_file = sample_two_original_copy
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
    # Changed attribute names to snake_case
    return ImageMetadata(
        source_file=sample_three_original_file,
        image_height=1000,
        image_width=1500,
        title=None,
        description='May 1, 2011\n"Much has been made of this photograph that shows the President and Vice President '
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
        region_info=RegionInfo(
            applied_to_dimensions=Dimensions(h=1000.0, w=1500.0, unit="pixel"),
            region_list=[
                Region(
                    area=XmpArea(h=0.072, unit="normalized", w=0.0386667, x=0.332667, y=0.39, d=None),
                    name="Barack Obama",
                    type_="Face",
                    description=None,
                ),
                Region(
                    area=XmpArea(h=0.079, unit="normalized", w=0.0386667, x=0.489333, y=0.1445, d=None),
                    name="Denis McDonough",
                    type_="Face",
                    description=None,
                ),
                Region(
                    area=XmpArea(h=0.108, unit="normalized", w=0.0626667, x=0.780667, y=0.484, d=None),
                    name="Hillary Clinton",
                    type_="Face",
                    description=None,
                ),
                Region(
                    area=XmpArea(h=0.094, unit="normalized", w=0.0433333, x=0.0916667, y=0.419, d=None),
                    name="Joseph R Biden",
                    type_="Face",
                    description=None,
                ),
            ],
        ),
        orientation=1,
        last_keyword_xmp=[
            "Locations/United States/Washington DC",
            "People/Barack Obama",
            "People/Denis McDonough",
            "People/Hillary Clinton",
            "People/Joseph R Biden",
        ],
        tags_list=[
            "Locations/United States/Washington DC",
            "People/Barack Obama",
            "People/Denis McDonough",
            "People/Hillary Clinton",
            "People/Joseph R Biden",
        ],
        catalog_sets=[
            "Locations|United States|Washington DC",
            "People|Barack Obama",
            "People|Denis McDonough",
            "People|Hillary Clinton",
            "People|Joseph R Biden",
        ],
        hierarchical_subject=[
            "Locations|United States|Washington DC",
            "People|Barack Obama",
            "People|Denis McDonough",
            "People|Hillary Clinton",
            "People|Joseph R Biden",
        ],
        keyword_info=KeywordInfo(
            hierarchy=[
                # Keyword(
                #     keyword="Locations",
                #     applied=None,
                #     children=[
                #         Keyword(
                #             keyword="United States",
                #             applied=None,
                #             children=[
                #                 Keyword(keyword="Washington DC", applied=None, children=[]),
                #             ],
                #         ),
                #     ],
                # ),
                # Keyword(
                #     keyword="People",
                #     applied=None,
                #     children=[
                #         Keyword(keyword="Joseph R Biden", applied=None, children=[]),
                #         Keyword(keyword="Denis McDonough", applied=None, children=[]),
                #         Keyword(keyword="Hillary Clinton", applied=None, children=[]),
                #         Keyword(keyword="Barack Obama", applied=None, children=[]),
                #     ],
                # ),
            ],
        ),
        country="USA",
        city="WASHINGTON",
        state="DC",
        location=None,
    )


@pytest.fixture
def sample_three_metadata_copy(
    sample_three_original_copy: Path,
    sample_three_metadata_original: ImageMetadata,
) -> ImageMetadata:
    # Changed attribute access to snake_case
    sample_three_metadata_original.source_file = sample_three_original_copy
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
    # Changed attribute names to snake_case
    return ImageMetadata(
        source_file=sample_four_original_file,
        image_height=436,
        image_width=654,
        title=None,
        description="CREATOR: gd-jpeg v1.0 (using IJG JPEG v62), quality = 100\n",
        region_info=RegionInfo(
            applied_to_dimensions=Dimensions(h=436.0, w=654.0, unit="pixel"),
            region_list=[
                Region(
                    area=XmpArea(h=0.0940367, unit="normalized", w=0.0428135, x=0.466361, y=0.186927, d=None),
                    name="Barack Obama",
                    type_="Face",
                    description=None,
                ),
            ],
        ),
        orientation=None,
        last_keyword_xmp=[
            "Locations/United States/Washington DC",
            "Pets/Dogs/Bo",
            "People/Barack Obama",
        ],
        tags_list=[
            "Locations/United States/Washington DC",
            "Pets/Dogs/Bo",
            "People/Barack Obama",
        ],
        catalog_sets=[
            "Locations|United States|Washington DC",
            "Pets|Dogs|Bo",
            "People|Barack Obama",
        ],
        hierarchical_subject=[
            "Locations|United States|Washington DC",
            "Pets|Dogs|Bo",
            "People|Barack Obama",
        ],
        keyword_info=KeywordInfo(
            hierarchy=[
                # Keyword(
                #     keyword="Locations",
                #     applied=None,
                #     children=[
                #         Keyword(
                #             keyword="United States",
                #             applied=None,
                #             children=[
                #                 Keyword(keyword="Washington DC", applied=None, children=[]),
                #             ],
                #         ),
                #     ],
                # ),
                # Keyword(
                #     keyword="Pets",
                #     applied=None,
                #     children=[
                #         Keyword(
                #             keyword="Dogs",
                #             applied=None,
                #             children=[Keyword(keyword="Bo", applied=None, children=[])],
                #         ),
                #     ],
                # ),
                # Keyword(
                #     keyword="People",
                #     applied=None,
                #     children=[Keyword(keyword="Barack Obama", applied=None, children=[])],
                # ),
            ],
        ),
        country=None,
        city=None,
        state=None,
        location=None,
    )


@pytest.fixture
def sample_four_metadata_copy(
    sample_four_original_copy: Path,
    sample_four_metadata_original: ImageMetadata,
) -> ImageMetadata:
    # Changed attribute access to snake_case
    sample_four_metadata_original.source_file = sample_four_original_copy
    return sample_four_metadata_original
