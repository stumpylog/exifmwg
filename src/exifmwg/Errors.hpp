#pragma once
#include <stdexcept>

class ExifMwgBaseError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class FileAccessError : public ExifMwgBaseError {
public:
  using ExifMwgBaseError::ExifMwgBaseError;
};

class Exiv2Error : public ExifMwgBaseError {
public:
  using ExifMwgBaseError::ExifMwgBaseError;
};

class InvalidStructureError : public ExifMwgBaseError {
public:
  using ExifMwgBaseError::ExifMwgBaseError;
};

class MissingFieldError : public InvalidStructureError {
public:
  using InvalidStructureError::InvalidStructureError;
};
