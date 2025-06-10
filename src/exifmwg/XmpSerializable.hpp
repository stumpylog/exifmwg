#pragma once
#include <concepts>
#include <exiv2/exiv2.hpp>

template <typename T>
concept XmpSerializable = requires(const T &obj, Exiv2::XmpData &xmpData,
                                   const Exiv2::XmpData &constXmpData) {
  { T::fromXmp(constXmpData) } -> std::same_as<T>;
  { obj.toXmp(xmpData) } -> std::same_as<void>;
};

template <typename T>
concept XmpSerializableWithKey =
    requires(const T &obj, Exiv2::XmpData &xmpData,
             const Exiv2::XmpData &constXmpData, const std::string &basePath) {
      { T::fromXmp(constXmpData, basePath) } -> std::same_as<T>;
      { obj.toXmp(xmpData, basePath) } -> std::same_as<void>;
    };
