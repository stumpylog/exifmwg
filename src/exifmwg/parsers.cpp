#include "parsers.hpp"

#include <stdexcept>

#include "DimensionsStruct.hpp"
#include "XmpAreaStruct.hpp"
#include "utils.hpp"

std::vector<std::string> parse_delimited_string(const Exiv2::XmpData &xmpData,
                                                const std::string &key,
                                                char delimiter) {
  std::vector<std::string> result;

  auto it = xmpData.findKey(Exiv2::XmpKey(key));
  if (it != xmpData.end()) {
    std::string value = it->toString();

    // Split on delimiter
    std::vector<std::string> tokens = split_string(value, delimiter);
    for (const auto &token : tokens) {
      std::string trimmed = trim(token);
      if (!trimmed.empty()) {
        result.push_back(trimmed);
      }
    }
  }

  return result;
}
