#include "utils.hpp"

#include <iostream>
#include <sstream>

#include <nanobind/nanobind.h>

namespace nb = nanobind;

void log_to_python(const std::string& level, const std::string& message)
{
    try
    {
        // Get Python's logging module
        nb::module_ logging = nb::module_::import_("logging");

        // Get the logger for this module
        nb::object logger = logging.attr("getLogger")("exifmwg");

        // Call the appropriate logging method
        if (level == "debug")
        {
            logger.attr("debug")(message);
        }
        else if (level == "info")
        {
            logger.attr("info")(message);
        }
        else if (level == "warning")
        {
            logger.attr("warning")(message);
        }
        else if (level == "error")
        {
            logger.attr("error")(message);
        }
        else
        {
            logger.attr("info")(message);
        }
    }
    catch (const std::exception& e)
    {
    }
    std::cerr << message << std::endl;
}

std::vector<std::string> split_string(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream        ss(str);
    std::string              token;
    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string clean_xmp_text(const std::string& xmpValue)
{
    // Handle XMP localized text format: lang="x-default" Actual text content
    std::string cleaned = xmpValue;

    // Look for the pattern lang="x-default" or similar language tags
    size_t langPos = cleaned.find("lang=\"");
    if (langPos != std::string::npos)
    {
        // Find the end of the language attribute (closing quote + space)
        size_t quoteEnd =
            cleaned.find("\"", langPos + 6); // 6 = length of "lang=\""
        if (quoteEnd != std::string::npos)
        {
            // Skip past the quote and any following whitespace
            size_t textStart = quoteEnd + 1;
            while (textStart < cleaned.length() &&
                   std::isspace(cleaned[textStart]))
            {
                textStart++;
            }
            cleaned = cleaned.substr(textStart);
        }
    }

    return cleaned;
}
