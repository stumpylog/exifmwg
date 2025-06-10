#include "utils.hpp"

#include <iostream>

#include <nanobind/nanobind.h>

namespace nb = nanobind;

void log_to_python(const std::string &level, const std::string &message) {
  // Get Python's logging module
  nb::module_ logging = nb::module_::import_("logging");

  // Get the logger for this module
  nb::object logger = logging.attr("getLogger")("exifmwg");

  // Call the appropriate logging method
  if (level == "debug") {
    logger.attr("debug")(message);
  } else if (level == "info") {
    logger.attr("info")(message);
  } else if (level == "warning") {
    logger.attr("warning")(message);
  } else if (level == "error") {
    logger.attr("error")(message);
  } else {
    logger.attr("info")(message);
  }
  std::cerr << message << std::endl;
}
