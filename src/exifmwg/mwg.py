# SPDX-FileCopyrightText: 2025-present Trenton H <rda0128ou@mozmail.com>
#
# SPDX-License-Identifier: MPL-2.0
import json
import logging
import tempfile
from pathlib import Path
from typing import TYPE_CHECKING
from typing import Any

from exiftool import ExifTool as ExiftoolBase

from exifmwg._utils import combine_keyword_structures
from exifmwg._utils import expand_keyword_structures
from exifmwg._utils import now_string
from exifmwg.errors import ImagePathNotFileError
from exifmwg.errors import NoImageMetadataError
from exifmwg.errors import NoImagePathsError
from exifmwg.models import ImageMetadata


class ExifTool(ExiftoolBase):
    def __init__(self, *args, **kwargs) -> None:  # type: ignore[no-untyped-def]
        super().__init__(*args, **kwargs)  # type: ignore[misc]
        # We want to control this more directly
        self.common_args = None

    def log(self, level: int, message: str) -> None:
        if self._logger is not None:
            if TYPE_CHECKING:  # type: ignore[unreachable]
                assert isinstance(self._logger, logging.Logger)
            self._logger.log(level, message)

    def read_image_metadata(self, image_path: Path) -> ImageMetadata:
        """
        Reads the requested metadata for a single image file
        """
        return self.bulk_read_image_metadata([image_path])[0]

    def bulk_read_image_metadata(self, image_paths: list[Path]) -> list[ImageMetadata]:
        """
        Reads the requested metadata for the given list of files.  This does a single subprocess call for
        all images at once, resulting in a more efficient method than looping through
        """

        if not image_paths:
            msg = "No image paths were provided"
            self.log(logging.ERROR, msg)
            raise NoImagePathsError(msg)

        cmd = [
            "-use",
            "MWG",
            "-struct",
            "-json",
            "-n",  # Disable print conversion, use machine readable formats
            "-ImageHeight",
            "-ImageWidth",
            # Face regions
            "-RegionInfo",
            "-MWG:Orientation",
            # Tags
            "-MWG:HierarchicalKeywords",
            "-XMP-microsoft:LastKeywordXMP",
            "-XMP-digiKam:TagsList",
            "-XMP-lr:HierarchicalSubject",
            "-XMP-mediapro:CatalogSets",
            # Other
            "-MWG:Description",
            "-Title",
            # Location
            "-MWG:Country",
            "-MWG:State",
            "-MWG:City",
            "-MWG:Location",
        ]

        # Add the actual images
        for image_path in image_paths:
            if not image_path.exists():
                msg = f"{image_path} does not exist"
                self.log(logging.ERROR, msg)
                raise FileNotFoundError(image_path)
            if not image_path.is_file():
                msg = f"{image_path} is not a file"
                self.log(logging.ERROR, msg)
                raise ImagePathNotFileError(msg)
            cmd.append(str(image_path.resolve()))

        # And run the command
        data = self.execute_json(*cmd)  # type: ignore[misc]

        return [combine_keyword_structures(y) for y in [ImageMetadata.model_validate(x) for x in data]]  # type: ignore[misc]

    def write_image_metadata(self, metadata: ImageMetadata, *, clear_existing_metadata: bool = False) -> None:
        """
        Updates the given SourceFile with the given metadata.  If a field has not been set,
        there will be no change to it.
        """
        return self.bulk_write_image_metadata([metadata], clear_existing_metadata=clear_existing_metadata)

    def bulk_write_image_metadata(
        self,
        metadata: list[ImageMetadata],
        *,
        clear_existing_metadata: bool = False,
    ) -> None:
        """
        Updates the given SourceFiles with the given metadata.  If a field has not been set,
        there will be no change to it.
        This does a single subprocess call, resulting is faster execution than looping
        """
        if not metadata:
            msg = "No image paths were provided"
            self.log(logging.ERROR, msg)
            raise NoImageMetadataError(msg)

        if clear_existing_metadata:
            self.bulk_clear_existing_metadata([x.SourceFile for x in metadata])

        with tempfile.TemporaryDirectory() as json_dir:
            json_path = Path(json_dir).resolve() / "temp.json"
            data: list[dict[str, Any]] = [
                expand_keyword_structures(x, self.log).model_dump(exclude_none=True, exclude_unset=True)  # type: ignore[misc]
                for x in metadata
            ]

            json_path.write_text(json.dumps(data))  # type: ignore[misc]
            cmd = [
                "-use",
                "MWG",
                "-struct",
                "-n",  # Disable print conversion, use machine readable
                "-overwrite_original",
                f"-MWG:ModifyDate={now_string()}",
                "-writeMode",
                "wcg",  # Create new tags/groups as necessary, overwrite existing
                f"-json={json_path}",
            ]
            # * unpacking doesn't resolve for the command?
            for x in metadata:
                cmd.append(str(x.SourceFile.resolve()))  # noqa: PERF401

            self.execute(*cmd)

    def clear_existing_metadata(self, image: Path) -> None:
        return self.bulk_clear_existing_metadata([image])

    def bulk_clear_existing_metadata(self, images: list[Path]) -> None:
        self.log(logging.DEBUG, "Clearing existing metadata")
        cmd = [
            "-overwrite_original",
            "-use",
            "MWG",
            "-v1",
            # Face regions
            "-XMP-mwg-rs:RegionInfo=",
            # Obvious
            "-MWG:Orientation=",
            # Tags
            "-XMP-mwg-kw:KeywordInfo=",
            "-XMP-acdsee:Categories=",
            "-HierarchicalKeywords=",
            "-XMP-microsoft:LastKeywordXMP=",
            "-XMP-digiKam:TagsList=",
            "-XMP-lr:HierarchicalSubject=",
            "-XMP-mediapro:CatalogSets=",
            # Other metadata
            "-Title=",
            "-MWG:Description=",
            "-XMP-dc:Description=",
            "-EXIF:ImageDescription=",
            "-IPTC:Caption-Abstract=",
            "-XMP-dc:Description-x-default=",
            "-IFD0:ImageDescription=",
            "-IPTC:Caption-Abstract=",
        ]
        for image in images:
            cmd.append(str(image.resolve()))  # noqa: PERF401

        self.log(logging.DEBUG, f"Running command '{' '.join(cmd)}'")
        self.execute(*cmd)
