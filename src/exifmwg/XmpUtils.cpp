#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

#include "XmpUtils.hpp"

namespace XmpUtils {

/**
 * @brief Clears XMP data entries whose keys contain a specified substring.
 *
 * This function iterates through the provided Exiv2::XmpData and removes
 * any entries where the key string contains the given 'key' substring.
 *
 * @param xmpData The Exiv2::XmpData object to modify.
 * @param key The substring to search for within XMP keys.
 */
void clearXmpKey(Exiv2::XmpData& xmpData, const std::string& key) {
  auto it = xmpData.begin();
  while (it != xmpData.end()) {
    if (it->key().find(key) != std::string::npos) {
      it = xmpData.erase(it);
    } else {
      ++it;
    }
  }
}

/**
 * @brief Converts a double-precision floating-point number to a string with a
 * specified precision.
 *
 * This function formats a double value into a string, fixing the number of
 * decimal places to the given precision.
 *
 * @param value The double value to convert.
 * @param precision The number of decimal places to include.
 * @return A string representation of the double with the specified precision.
 */
std::string doubleToStringWithPrecision(double value, int precision) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(precision) << value;
  std::string result = oss.str();

  // If there's a decimal point, trim trailing zeros
  auto dot_pos = result.find('.');
  if (dot_pos != std::string::npos) {
    // Remove trailing zeros
    size_t last_nonzero = result.find_last_not_of('0');
    if (last_nonzero != std::string::npos && last_nonzero > dot_pos) {
      result.erase(last_nonzero + 1);
    } else {
      // All zeros after the decimal (e.g., "0.00000") → preserve one zero:
      // "0.0"
      result.erase(dot_pos + 2); // keep one digit after '.'
    }
  }

  return result;
}
/**
 * @brief Trims leading and trailing whitespace characters from a string.
 *
 * This function removes spaces, tabs, newlines, and carriage returns from
 * the beginning and end of the input string.
 *
 * @param str The string to trim.
 * @return The trimmed string. Returns an empty string if the input is all
 * whitespace.
 */
std::string trimWhitespace(const std::string& str) {
  size_t first = str.find_first_not_of(" \t\n\r");
  if (first == std::string::npos) {
    return "";
  }
  size_t last = str.find_last_not_of(" \t\n\r");
  return str.substr(first, (last - first + 1));
}

/**
 * @brief Splits a string into a vector of substrings based on a delimiter.
 *
 * This function parses the input string and breaks it into individual tokens
 * wherever the specified delimiter character is found.
 *
 * @param str The string to split.
 * @param delimiter The character used to delimit tokens.
 * @return A std::vector of strings, where each element is a token.
 */
std::vector<std::string> splitString(const std::string& str, char delimiter) {
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, delimiter)) {
    if (!token.empty()) {
      tokens.push_back(token);
    }
  }
  return tokens;
}

/**
 * @brief Cleans an XMP text value, specifically handling localized text format.
 *
 * This function processes XMP text values. If the value follows the
 * `lang="x-default" Actual text content` pattern (or similar language tags),
 * it extracts and returns only the "Actual text content". Otherwise, it returns
 * the original string.
 *
 * @param xmpValue The XMP text string to clean.
 * @return The cleaned XMP text content.
 */
std::string cleanXmpText(const std::string& xmpValue) {

  // 6 = length of "lang=\""
  constexpr size_t LANG_ATTRIBUTE_LENGTH = 6;

  // Handle XMP localized text format: lang="x-default" Actual text content
  std::string cleaned = xmpValue;

  // Look for the pattern lang=""
  size_t langPos = cleaned.find("lang=\"");
  if (langPos != std::string::npos) {
    // Find the end of the language attribute (closing quote)
    size_t quoteEnd = cleaned.find('"', langPos + LANG_ATTRIBUTE_LENGTH);
    if (quoteEnd != std::string::npos) {
      // Skip past the quote and any following whitespace
      size_t textStart = quoteEnd + 1;
      while (textStart < cleaned.length() && std::isspace(cleaned[textStart]) != 0) {
        textStart++;
      }
      cleaned = cleaned.substr(textStart);
    }
  }

  return cleaned;
}

/**
 * @brief Parses a delimited string value from XMP data.
 *
 * This function retrieves the string value associated with a given XMP key,
 * splits it by a specified delimiter, trims whitespace from each resulting
 * token, and returns a vector of non-empty, trimmed tokens.
 *
 * @param xmpData The Exiv2::XmpData object to query.
 * @param key The XMP key whose value is to be parsed.
 * @param delimiter The character used to delimit tokens within the XMP value
 * string.
 * @return A std::vector of cleaned and trimmed strings extracted from the XMP
 * value.
 */
std::vector<std::string> parseDelimitedString(const Exiv2::XmpData& xmpData, const std::string& key, char delimiter) {
  std::vector<std::string> result;

  auto it = xmpData.findKey(Exiv2::XmpKey(key));
  if (it != xmpData.end()) {
    std::string value = it->toString();

    // Split on delimiter
    std::vector<std::string> tokens = splitString(value, delimiter);
    for (const auto& token : tokens) {
      std::string trimmed = trimWhitespace(token);
      if (!trimmed.empty()) {
        result.push_back(trimmed);
      }
    }
  }

  return result;
}

std::string joinStrings(const std::vector<std::string>& vec, char delimiter) {
  if (vec.empty()) {
    return "";
  }
  std::ostringstream oss;
  for (size_t i = 0; i < vec.size(); ++i) {
    if (i > 0) {
      oss << delimiter;
    }
    oss << vec[i];
  }
  return oss.str();
}

} // namespace XmpUtils
