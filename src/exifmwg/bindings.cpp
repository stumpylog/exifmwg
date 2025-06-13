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
#include <nanobind/stl/filesystem.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "DimensionsStruct.hpp"
#include "Image.hpp"
#include "ImageMetadata.hpp"
#include "KeywordInfoModel.hpp"
#include "RegionInfoStruct.hpp"
#include "XmpAreaStruct.hpp"
#include "clearing.hpp"
#include "reading.hpp"
#include "writing.hpp"

namespace nb = nanobind;
namespace fs = std::filesystem;

using namespace nb::literals;

NB_MODULE(bindings, m) {
  m.doc() = "C++ bindings to Exiv2 for reading and writing MWG information";
  nb::class_<ImageMetadata>(m, "ImageMetadata")
      .def(
          nb::init<int, int, std::optional<std::string>, std::optional<std::string>, std::optional<RegionInfoStruct>,
                   std::optional<int>, std::optional<std::vector<std::string>>, std::optional<std::vector<std::string>>,
                   std::optional<std::vector<std::string>>, std::optional<std::vector<std::string>>,
                   std::optional<KeywordInfoModel>, std::optional<std::string>, std::optional<std::string>,
                   std::optional<std::string>, std::optional<std::string>>(),
          "image_height"_a, "image_width"_a, "title"_a = nb::none(), "description"_a = nb::none(),
          "region_info"_a = nb::none(), "orientation"_a = nb::none(), "last_keyword_xmp"_a = nb::none(),
          "tags_list"_a = nb::none(), "catalog_sets"_a = nb::none(), "hierarchical_subject"_a = nb::none(),
          "keyword_info"_a = nb::none(), "country"_a = nb::none(), "city"_a = nb::none(), "state"_a = nb::none(),
          "location"_a = nb::none())
      .def("__eq__", [](const ImageMetadata& self, const ImageMetadata& other) { return self == other; })
      .def("__ne__", [](const ImageMetadata& self, const ImageMetadata& other) { return self != other; })
      .def_rw("image_height", &ImageMetadata::ImageHeight)
      .def_rw("image_width", &ImageMetadata::ImageWidth)
      .def_rw("title", &ImageMetadata::Title)
      .def_rw("description", &ImageMetadata::Description)
      .def_rw("region_info", &ImageMetadata::RegionInfo)
      .def_rw("orientation", &ImageMetadata::Orientation)
      .def_rw("last_keyword_xmp", &ImageMetadata::LastKeywordXMP)
      .def_rw("tags_list", &ImageMetadata::TagsList)
      .def_rw("catalog_sets", &ImageMetadata::CatalogSets)
      .def_rw("hierarchical_subject", &ImageMetadata::HierarchicalSubject)
      .def_rw("keyword_info", &ImageMetadata::KeywordInfo)
      .def_rw("country", &ImageMetadata::Country)
      .def_rw("city", &ImageMetadata::City)
      .def_rw("state", &ImageMetadata::State)
      .def_rw("location", &ImageMetadata::Location);
  nb::class_<Image>(m, "Image",
                    "Represents an image file and provides methods for reading, writing, and clearing "
                    "its metadata (e.g., XMP, EXIF). "
                    "Metadata fields are exposed as properties that can be directly accessed and modified. "
                    "Changes to these properties are not saved until the `save()` method is called.")
      .def(nb::init<std::filesystem::path&>(), "file_path"_a,
           "Constructs an Image object by reading metadata from the given path.")
      .def(
          "__eq__", [](const Image& self, const Image& other) { return self == other; },
          "Compares two Image objects for equality.")
      .def(
          "__ne__", [](const Image& self, const Image& other) { return self != other; },
          "Compares two Image objects for inequality.")
      .def("save", &Image::save, "new_path"_a = nb::none(),
           "Saves metadata changes back to a file. "
           "If `new_path` is provided, the original image is copied to the new location "
           "and the metadata is written to the new file. Otherwise, it overwrites "
           "the original file with the updated metadata.")
      .def("clear_metadata", &Image::clearMetadata,
           "Clears all supported metadata fields from the object and saves the changes "
           "back to the original file. This is a destructive operation.")
      .def_prop_ro("path", &Image::getPath, "Get the path to the image file as a read-only property.")
      .def_rw("image_height", &Image::ImageHeight, "The height of the image in pixels.")
      .def_rw("image_width", &Image::ImageWidth, "The width of the image in pixels.")
      .def_rw("title", &Image::Title,
              "The intellectual graphic title for the image, often corresponding to `dc:title` in XMP.")
      .def_rw("description", &Image::Description,
              "A textual description of the image, often corresponding to `dc:description` in XMP.")
      .def_rw("region_info", &Image::RegionInfo,
              "Information about regions (e.g., faces) within the image, including their dimensions, "
              "names, and types. Corresponds to `mwg-rs:Regions` in XMP.")
      .def_rw("orientation", &Image::Orientation,
              "The orientation of the image, typically an EXIF orientation tag (e.g., 1 for normal, "
              "6 for rotated 90 degrees clockwise).")
      .def_rw("last_keyword_xmp", &Image::LastKeywordXMP,
              "The last keyword applied to the image, often corresponding to `digiKam:TagsList` or similar in XMP, "
              "though its exact meaning can vary.")
      .def_rw("tags_list", &Image::TagsList,
              "A list of tags or keywords associated with the image, often corresponding to `dc:subject` "
              "or `photoshop:SupplementalCategories` in XMP.")
      .def_rw("catalog_sets", &Image::CatalogSets,
              "The catalog sets or categories the image belongs to, typically used in photo management software. "
              "Corresponds to `microsoft:CatalogSets` in XMP.")
      .def_rw("hierarchical_subject", &Image::HierarchicalSubject,
              "Hierarchical keywords or subjects, representing a categorized structure for keywords. "
              "Corresponds to `lr:hierarchicalSubject` in XMP.")
      .def_rw("keyword_info", &Image::KeywordInfo,
              "Detailed keyword information, potentially including hierarchical structures and "
              "application status. This is a more structured representation than `tags_list` "
              "or `hierarchical_subject`.")
      .def_rw("country", &Image::Country,
              "The country where the image was taken or is associated with. Corresponds to "
              "`Iptc4xmpExt:CountryName` or `photoshop:Country` in XMP.")
      .def_rw("city", &Image::City,
              "The city where the image was taken or is associated with. Corresponds to "
              "`Iptc4xmpCore:City` or `photoshop:City` in XMP.")
      .def_rw("state", &Image::State,
              "The state/province where the image was taken or is associated with. Corresponds to "
              "`Iptc4xmpCore:ProvinceState` or `photoshop:State` in XMP.")
      .def_rw("location", &Image::Location,
              "A general location description for the image, often more specific than just city/state/country. "
              "Corresponds to `Iptc4xmpCore:Location` or `photoshop:Location` in XMP.");

  nb::class_<XmpAreaStruct>(m, "XmpArea")
      .def(nb::init<double, double, double, double, const std::string&, std::optional<double>>(), "h"_a, "w"_a, "x"_a,
           "y"_a, "unit"_a, "d"_a = nb::none())
      .def("__eq__", [](const XmpAreaStruct& self, const XmpAreaStruct& other) { return self == other; })
      .def_rw("h", &XmpAreaStruct::H)
      .def_rw("w", &XmpAreaStruct::W)
      .def_rw("x", &XmpAreaStruct::X)
      .def_rw("y", &XmpAreaStruct::Y)
      .def_rw("d", &XmpAreaStruct::D)
      .def_rw("unit", &XmpAreaStruct::Unit);

  nb::class_<DimensionsStruct>(m, "Dimensions")
      .def(nb::init<double, double, const std::string&>(), "h"_a, "w"_a, "unit"_a)
      .def("__eq__", [](const DimensionsStruct& self, const DimensionsStruct& other) { return self == other; })
      .def_rw("h", &DimensionsStruct::H)
      .def_rw("w", &DimensionsStruct::W)
      .def_rw("unit", &DimensionsStruct::Unit);

  nb::class_<RegionInfoStruct::RegionStruct>(m, "Region")
      .def(nb::init<const XmpAreaStruct&, const std::string&, const std::string&, std::optional<std::string>>(),
           "area"_a, "name"_a, "type_"_a, "description"_a = nb::none())
      .def("__eq__", [](const RegionInfoStruct::RegionStruct& self,
                        const RegionInfoStruct::RegionStruct& other) { return self == other; })
      .def_rw("area", &RegionInfoStruct::RegionStruct::Area)
      .def_rw("name", &RegionInfoStruct::RegionStruct::Name)
      .def_rw("type", &RegionInfoStruct::RegionStruct::Type)
      .def_rw("description", &RegionInfoStruct::RegionStruct::Description);

  nb::class_<RegionInfoStruct>(m, "RegionInfo")
      .def(nb::init<const DimensionsStruct&, const std::vector<RegionInfoStruct::RegionStruct>&>(),
           "applied_to_dimensions"_a, "region_list"_a)
      .def("__eq__", [](const RegionInfoStruct& self, const RegionInfoStruct& other) { return self == other; })
      .def_rw("applied_to_dimensions", &RegionInfoStruct::AppliedToDimensions)
      .def_rw("region_list", &RegionInfoStruct::RegionList);

  nb::class_<KeywordInfoModel::KeywordStruct>(m, "Keyword")
      .def(nb::init<const std::string&, const std::vector<KeywordInfoModel::KeywordStruct>&, std::optional<bool>>(),
           "keyword"_a, "children"_a, "applied"_a = nb::none())
      .def("__eq__", [](const KeywordInfoModel::KeywordStruct& self,
                        const KeywordInfoModel::KeywordStruct& other) { return self == other; })
      .def_rw("keyword", &KeywordInfoModel::KeywordStruct::Keyword)
      .def_rw("applied", &KeywordInfoModel::KeywordStruct::Applied)
      .def_rw("children", &KeywordInfoModel::KeywordStruct::Children);

  nb::class_<KeywordInfoModel>(m, "KeywordInfo")
      .def(nb::init<const std::vector<KeywordInfoModel::KeywordStruct>&>(), "hierarchy"_a)
      .def(nb::init<const std::vector<std::string>&>(), "delimited_strings"_a)
      .def(nb::init<const std::vector<std::string>&, char>(), "delimited_strings"_a, "delimiter"_a)
      .def("__eq__", [](const KeywordInfoModel& self, const KeywordInfoModel& other) { return self == other; })
      .def("__or__", [](const KeywordInfoModel& self, const KeywordInfoModel& other) { return self | other; })
      .def(
          "__ior__",
          [](KeywordInfoModel& self, const KeywordInfoModel& other) -> KeywordInfoModel& { return self |= other; },
          nb::rv_policy::reference_internal)
      .def_rw("hierarchy", &KeywordInfoModel::Hierarchy);
  m.def("read_metadata", &read_metadata, "Read metadata from an image file");
  m.def("write_metadata", &write_metadata, "Write metadata to an image file");
  m.def("clear_existing_metadata", &clear_existing_metadata, "Clear existing metadata from an image file");
  m.def(
      "exiv2_version", []() -> std::string { return Exiv2::versionString(); },
      "Returns the Exiv2 library version string.");
  m.def(
      "expat_version", []() -> std::string { return XML_ExpatVersion(); },
      "Returns the libexpat library version string.");
}
