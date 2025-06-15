#pragma once

#include <concepts>
#include <string>

template <typename T>
concept PythonBindable = requires(const T& a, const T& b) {
  // Requires a string representation method
  { a.to_string() } -> std::convertible_to<std::string>;

  // Requires equality comparison
  { a == b } -> std::convertible_to<bool>;

  // Requires inequality comparison
  { a != b } -> std::convertible_to<bool>;
};
