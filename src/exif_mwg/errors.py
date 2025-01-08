# SPDX-FileCopyrightText: 2025-present Trenton H <rda0128ou@mozmail.com>
#
# SPDX-License-Identifier: MPL-2.0


class ExifMwgBaseError(Exception):
    """
    Base exception for all errors which arise from this library
    """


class NoImagePathsError(ExifMwgBaseError):
    """
    A metadata read operation did not include an images to read
    """


class NoImageMetadataError(ExifMwgBaseError):
    """
    No metadata was provided to a metadata write operation
    """


class ImagePathNotFileError(ExifMwgBaseError):
    """
    A provided image path is not a file
    """
