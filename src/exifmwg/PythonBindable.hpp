#pragma once

#include <concepts>
#include <string>

template <typename T>
concept PythonBindableRepr = requires(const T& a, const T& b) {
  // Requires a string representation method
  { a.to_string() } -> std::convertible_to<std::string>;

  // Requires equality comparison
  { a == b } -> std::convertible_to<bool>;

  // Requires inequality comparison
  { a != b } -> std::convertible_to<bool>;
};

// Concept for container-like objects
template <typename T>
concept PythonBindableContainer = requires(const T& a, T& b, std::size_t i) {
  // Size/length
  { a.size() } -> std::convertible_to<std::size_t>;
  { a.empty() } -> std::convertible_to<bool>;

  // Iteration support
  { a.begin() };
  { a.end() };
};
