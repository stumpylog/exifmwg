#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "exiv2/exiv2.hpp"

#include <nanobind/nanobind.h>
#include <nanobind/stl/filesystem.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "clearing.hpp"
#include "models.hpp"
#include "parsers.hpp"
#include "reading.hpp"
#include "utils.hpp"
#include "writing.hpp"

namespace nb = nanobind;
namespace fs = std::filesystem;

using namespace nb::literals;

NB_MODULE(exifmwg, m)
{
    nb::class_<ImageMetadata>(m, "ImageMetadata")
        .def(
            nb::init<
                int,
                int,
                std::optional<std::string>,
                std::optional<std::string>,
                std::optional<RegionInfoStruct>,
                std::optional<int>,
                std::optional<std::vector<std::string>>,
                std::optional<std::vector<std::string>>,
                std::optional<std::vector<std::string>>,
                std::optional<std::vector<std::string>>,
                std::optional<KeywordInfoModel>,
                std::optional<std::string>,
                std::optional<std::string>,
                std::optional<std::string>,
                std::optional<std::string>>(),
            "image_height"_a,
            "image_width"_a,
            "title"_a                = nb::none(),
            "description"_a          = nb::none(),
            "region_info"_a          = nb::none(),
            "orientation"_a          = nb::none(),
            "last_keyword_xmp"_a     = nb::none(),
            "tags_list"_a            = nb::none(),
            "catalog_sets"_a         = nb::none(),
            "hierarchical_subject"_a = nb::none(),
            "keyword_info"_a         = nb::none(),
            "country"_a              = nb::none(),
            "city"_a                 = nb::none(),
            "state"_a                = nb::none(),
            "location"_a             = nb::none())
        .def(
            "__eq__",
            [](const ImageMetadata& self, const ImageMetadata& other)
            { return self == other; })
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

    nb::class_<XmpAreaStruct>(m, "XmpArea")
        .def(
            nb::init<
                double,
                double,
                double,
                double,
                const std::string&,
                std::optional<double>>(),
            "h"_a,
            "w"_a,
            "x"_a,
            "y"_a,
            "unit"_a,
            "d"_a = nb::none())
        .def(
            "__eq__",
            [](const XmpAreaStruct& self, const XmpAreaStruct& other)
            { return self == other; })
        .def_rw("h", &XmpAreaStruct::H)
        .def_rw("w", &XmpAreaStruct::W)
        .def_rw("x", &XmpAreaStruct::X)
        .def_rw("y", &XmpAreaStruct::Y)
        .def_rw("d", &XmpAreaStruct::D)
        .def_rw("unit", &XmpAreaStruct::Unit);

    nb::class_<DimensionsStruct>(m, "Dimensions")
        .def(
            nb::init<double, double, const std::string&>(),
            "h"_a,
            "w"_a,
            "unit"_a)
        .def(
            "__eq__",
            [](const DimensionsStruct& self, const DimensionsStruct& other)
            { return self == other; })
        .def_rw("h", &DimensionsStruct::H)
        .def_rw("w", &DimensionsStruct::W)
        .def_rw("unit", &DimensionsStruct::Unit);

    nb::class_<RegionStruct>(m, "Region")
        .def(
            nb::init<
                const XmpAreaStruct&,
                const std::string&,
                const std::string&,
                std::optional<std::string>>(),
            "area"_a,
            "name"_a,
            "type_"_a,
            "description"_a = nb::none())
        .def(
            "__eq__",
            [](const RegionStruct& self, const RegionStruct& other)
            { return self == other; })
        .def_rw("area", &RegionStruct::Area)
        .def_rw("name", &RegionStruct::Name)
        .def_rw("type", &RegionStruct::Type)
        .def_rw("description", &RegionStruct::Description);

    nb::class_<RegionInfoStruct>(m, "RegionInfo")
        .def(
            nb::init<
                const DimensionsStruct&,
                const std::vector<RegionStruct>&>(),
            "applied_to_dimensions"_a,
            "region_list"_a)
        .def(
            "__eq__",
            [](const RegionInfoStruct& self, const RegionInfoStruct& other)
            { return self == other; })
        .def_rw("applied_to_dimensions", &RegionInfoStruct::AppliedToDimensions)
        .def_rw("region_list", &RegionInfoStruct::RegionList);

    nb::class_<KeywordStruct>(m, "Keyword")
        .def(
            nb::init<
                const std::string&,
                const std::vector<KeywordStruct>&,
                std::optional<bool>>(),
            "keyword"_a,
            "children"_a,
            "applied"_a = nb::none())
        .def(
            "__eq__",
            [](const KeywordStruct& self, const KeywordStruct& other)
            { return self == other; })
        .def_rw("keyword", &KeywordStruct::Keyword)
        .def_rw("applied", &KeywordStruct::Applied)
        .def_rw("children", &KeywordStruct::Children);

    nb::class_<KeywordInfoModel>(m, "KeywordInfo")
        .def(nb::init<const std::vector<KeywordStruct>&>(), "hierarchy"_a)
        .def(nb::init<const std::vector<std::string>&>(), "delimited_strings"_a)
        .def(
            nb::init<const std::vector<std::string>&, char>(),
            "delimited_strings"_a,
            "delimiter"_a)
        .def(
            "__eq__",
            [](const KeywordInfoModel& self, const KeywordInfoModel& other)
            { return self == other; })
        .def(
            "__or__",
            [](const KeywordInfoModel& self, const KeywordInfoModel& other)
            { return self | other; })
        .def(
            "__ior__",
            [](KeywordInfoModel&       self,
               const KeywordInfoModel& other) -> KeywordInfoModel&
            { return self |= other; },
            nb::rv_policy::reference_internal)
        .def_rw("hierarchy", &KeywordInfoModel::Hierarchy);
    m.def("read_metadata", &read_metadata, "Read metadata from an image file");
    m.def("write_metadata", &write_metadata, "Write metadata to an image file");
    m.def(
        "clear_existing_metadata",
        &clear_existing_metadata,
        "Clear existing metadata from an image file");
}
