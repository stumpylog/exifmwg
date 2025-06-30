#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <exiv2/exiv2.hpp>
#include <expat.h>

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/filesystem.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "DimensionsStruct.hpp"
#include "Errors.hpp"
#include "ImageMetadata.hpp"
#include "KeywordInfoModel.hpp"
#include "Logging.hpp"
#include "Orientation.hpp"
#include "RegionInfoStruct.hpp"
#include "XmpAreaStruct.hpp"

namespace nb = nanobind;
namespace fs = std::filesystem;

using namespace nb::literals;

NB_MODULE(bindings, m) {
  m.doc() = "C++ bindings to Exiv2 for reading and writing MWG information";
  nb::class_<ImageMetadata>(m, "ImageMetadata")
      .def(nb::init<int, int, std::optional<std::string>, std::optional<std::string>, std::optional<RegionInfoStruct>,
                    std::optional<ExifOrientation>, std::optional<KeywordInfoModel>, std::optional<std::string>,
                    std::optional<std::string>, std::optional<std::string>, std::optional<std::string>>(),
           "image_height"_a, "image_width"_a, "title"_a = nb::none(), "description"_a = nb::none(),
           "region_info"_a = nb::none(), "orientation"_a = nb::none(), "keyword_info"_a = nb::none(),
           "country"_a = nb::none(), "city"_a = nb::none(), "state"_a = nb::none(), "location"_a = nb::none())
      .def(nb::init<const fs::path&>(), "path"_a)
      .def(nb::self == nb::self) // operator==
      .def(nb::self != nb::self) // operator!=
      .def("__repr__", &ImageMetadata::to_string)
      .def("to_file", &ImageMetadata::toFile, "new_path"_a = nb::none(),
           "If `new_path` is provided, the original image is copied to the new location "
           "and the metadata is written to the new file. Otherwise, it overwrites "
           "the original file with the updated metadata.")
      .def_ro("image_height", &ImageMetadata::ImageHeight)
      .def_ro("image_width", &ImageMetadata::ImageWidth)
      .def_rw("title", &ImageMetadata::Title)
      .def_rw("description", &ImageMetadata::Description)
      .def_rw("region_info", &ImageMetadata::RegionInfo)
      .def_rw("orientation", &ImageMetadata::Orientation)
      .def_rw("keyword_info", &ImageMetadata::KeywordInfo)
      .def_rw("country", &ImageMetadata::Country)
      .def_rw("city", &ImageMetadata::City)
      .def_rw("state", &ImageMetadata::State)
      .def_rw("location", &ImageMetadata::Location);

  nb::enum_<ExifOrientation>(m, "ExifOrientation", nb::is_arithmetic())
      .value("Undefined", ExifOrientation::Undefined, "Set but not a valid value")
      .value("Horizontal", ExifOrientation::Horizontal, "Normal (0° rotation)")
      .value("TopLeft", ExifOrientation::TopLeft, "Normal (0° rotation)")
      .value("MirrorHorizontal", ExifOrientation::MirrorHorizontal, "Horizontal flip")
      .value("TopRight", ExifOrientation::TopRight, "Horizontal flip")
      .value("Rotate180", ExifOrientation::Rotate180, "180° rotation")
      .value("BottomRight", ExifOrientation::BottomRight, "180° rotation")
      .value("MirrorVertical", ExifOrientation::MirrorVertical, "Vertical flip")
      .value("BottomLeft", ExifOrientation::BottomLeft, "Vertical flip")
      .value("MirrorHorizontalAndRotate270CW", ExifOrientation::MirrorHorizontalAndRotate270CW,
             "90° CCW rotation + horizontal flip")
      .value("LeftTop", ExifOrientation::LeftTop, "90° CCW rotation + horizontal flip")
      .value("Rotate90CW", ExifOrientation::Rotate90CW, "90° CW rotation")
      .value("RightTop", ExifOrientation::RightTop, "90° CW rotation")
      .value("MirrorHorizontalAndRotate90CW", ExifOrientation::MirrorHorizontalAndRotate90CW,
             "90° CW rotation + horizontal flip")
      .value("RightBottom", ExifOrientation::RightBottom, "90° CW rotation + horizontal flip")
      .value("Rotate270CW", ExifOrientation::Rotate270CW, "90° CCW rotation")
      .value("LeftBottom", ExifOrientation::LeftBottom, "90° CCW rotation")
      .def("to_exif_value", &orientation_to_exif_value, "Convert to EXIF orientation value")
      .def("__int__", &orientation_to_int, "Convert to integer")
      .def("__str__", &orientation_to_string, "String representation")
      .def("__repr__", &orientation_to_string, "String representation")
      .def_static("from_exif_value", &orientation_from_exif_value, "Create from EXIF orientation value")
      .def_static("from_int", &orientation_from_int, "Create from integer");

  nb::class_<XmpAreaStruct>(m, "XmpArea")
      .def(nb::init<double, double, double, double, const std::string&, std::optional<double>>(), "h"_a, "w"_a, "x"_a,
           "y"_a, "unit"_a, "d"_a = nb::none())
      .def(nb::self == nb::self) // operator==
      .def(nb::self != nb::self) // operator!=
      .def("__repr__", &XmpAreaStruct::to_string)
      .def_rw("h", &XmpAreaStruct::H)
      .def_rw("w", &XmpAreaStruct::W)
      .def_rw("x", &XmpAreaStruct::X)
      .def_rw("y", &XmpAreaStruct::Y)
      .def_rw("d", &XmpAreaStruct::D)
      .def_rw("unit", &XmpAreaStruct::Unit);

  nb::class_<DimensionsStruct>(m, "Dimensions")
      .def(nb::init<double, double, const std::string&>(), "h"_a, "w"_a, "unit"_a)
      .def(nb::self == nb::self) // operator==
      .def(nb::self != nb::self) // operator!=
      .def(nb::hash(nb::self))
      .def("__repr__", &DimensionsStruct::to_string)
      .def_rw("h", &DimensionsStruct::H)
      .def_rw("w", &DimensionsStruct::W)
      .def_rw("unit", &DimensionsStruct::Unit);

  nb::class_<RegionInfoStruct::RegionStruct>(m, "Region")
      .def(nb::init<const XmpAreaStruct&, const std::string&, const std::string&, std::optional<std::string>>(),
           "area"_a, "name"_a, "type_"_a, "description"_a = nb::none())
      .def(nb::self == nb::self) // operator==
      .def(nb::self != nb::self) // operator!=
      .def("__repr__", &RegionInfoStruct::RegionStruct::to_string)
      .def_rw("area", &RegionInfoStruct::RegionStruct::Area)
      .def_rw("name", &RegionInfoStruct::RegionStruct::Name)
      .def_rw("type", &RegionInfoStruct::RegionStruct::Type)
      .def_rw("description", &RegionInfoStruct::RegionStruct::Description);

  nb::class_<RegionInfoStruct>(m, "RegionInfo")
      .def(nb::init<const DimensionsStruct&, const std::vector<RegionInfoStruct::RegionStruct>&>(),
           "applied_to_dimensions"_a, "region_list"_a)
      .def(nb::self == nb::self) // operator==
      .def(nb::self != nb::self) // operator!=
      .def("__repr__", &RegionInfoStruct::to_string)
      .def_rw("applied_to_dimensions", &RegionInfoStruct::AppliedToDimensions)
      .def_rw("region_list", &RegionInfoStruct::RegionList);

  nb::class_<KeywordInfoModel::KeywordStruct>(m, "Keyword")
      .def(nb::init<const std::string&, const std::vector<KeywordInfoModel::KeywordStruct>&, std::optional<bool>>(),
           "keyword"_a, "children"_a, "applied"_a = nb::none())
      .def(nb::self < nb::self)  // operator<
      .def(nb::self <= nb::self) // operator<=
      .def(nb::self > nb::self)  // operator>
      .def(nb::self >= nb::self) // operator>=
      .def(nb::self == nb::self) // operator==
      .def(nb::self != nb::self) // operator!=
      .def("__repr__", &KeywordInfoModel::KeywordStruct::to_string)
      .def_rw("keyword", &KeywordInfoModel::KeywordStruct::Keyword)
      .def_rw("applied", &KeywordInfoModel::KeywordStruct::Applied)
      .def_rw("children", &KeywordInfoModel::KeywordStruct::Children);

  nb::class_<KeywordInfoModel>(m, "KeywordInfo")
      .def(nb::init<const std::vector<KeywordInfoModel::KeywordStruct>&>(), "hierarchy"_a)
      .def(nb::init<const std::vector<std::string>&>(), "delimited_strings"_a)
      .def(nb::init<const std::vector<std::string>&, char>(), "delimited_strings"_a, "delimiter"_a)
      .def(nb::self == nb::self)                                    // operator==
      .def(nb::self != nb::self)                                    // operator!=
      .def(nb::self | nb::self)                                     // operator|
      .def(nb::self |= nb::self, nb::rv_policy::reference_internal) // operator|=
      .def("__repr__", &KeywordInfoModel::to_string)
      .def_rw("hierarchy", &KeywordInfoModel::Hierarchy);
  m.attr("EXIV2_VERSION") = Exiv2::versionString();
  m.attr("EXPAT_VERSION") = XML_ExpatVersion();
  // Register base exception first
  auto base_exc = nb::exception<ExifMwgBaseError>(m, "ExifMwgBaseError");
  // Register derived exceptions with explicit parent
  nb::exception<FileAccessError>(m, "FileAccessError", base_exc.ptr());
  nb::exception<Exiv2Error>(m, "Exiv2Error", base_exc.ptr());
  auto invalid_struct_exc = nb::exception<InvalidStructureError>(m, "InvalidStructureError", base_exc.ptr());
  // Register nested derived exception with its immediate parent
  nb::exception<MissingFieldError>(m, "MissingFieldError", invalid_struct_exc.ptr());
  PythonLogger::init();
}
