#pragma once

#include <cstring>
#include <iostream>
#include <string>

#ifdef NANOBIND_MODULE
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h> // Required for nb::str
namespace nb = nanobind;
#endif

constexpr const char* _moduleName = "exifmwg.bindings";

#ifdef NANOBIND_MODULE
struct PythonLogger {
private:
  // Static pointer to hold the Python logger object.
  static inline nb::object* py_logger = nullptr;

public:
  // Must be called from your main NB_MODULE block to initialize the logger.
  static void init() {
    try {
      // Acquire the GIL before any interaction with Python objects.
      nb::gil_scoped_acquire gil;
      auto logging = nb::module_::import_("logging");

      // Allocate the logger on the heap to avoid static deinitialization issues.
      // It will be cleaned up when the interpreter shuts down.
      py_logger = new nb::object(logging.attr("getLogger")(_moduleName));

    } catch (const std::exception& e) {
      std::cerr << "Fatal: Failed to initialize Python logger: " << e.what() << std::endl;
      throw;
    }
  }

  static void log(const char* level, const std::string& msg) {
    // If the logger was not initialized, we can't proceed.
    if (!py_logger) {
      std::cerr << "[ERROR] Logger not initialized. Log message: " << msg << std::endl;
      return;
    }

    try {
      // Acquire the GIL before calling into Python.
      nb::gil_scoped_acquire gil;

      // Let nanobind's type caster handle the std::string -> Python str conversion implicitly.
      py_logger->attr(level)(msg);

    } catch (const std::exception& e) {
      // Fallback to stderr if the Python call fails for some reason.
      std::cerr << "[" << level << "] " << _moduleName << ": " << msg << std::endl;
      std::cerr << "Python logging error: " << e.what() << std::endl;
    }
  }
};
#else
struct NativeLogger {
  static void log(const char* level, const std::string& msg) {
    auto& stream = (std::strcmp(level, "warning") == 0 || std::strcmp(level, "error") == 0) ? std::cerr : std::cout;
    stream << "[" << level << "] " << _moduleName << ": " << msg << std::endl;
  }
};
#endif

class InternalLogger {
private:
#ifdef NANOBIND_MODULE
  using LoggerImpl = PythonLogger;
#else
  using LoggerImpl = NativeLogger;
#endif

public:
  static void debug(const std::string& msg) {
    LoggerImpl::log("debug", msg);
  }
  static void info(const std::string& msg) {
    LoggerImpl::log("info", msg);
  }
  static void warning(const std::string& msg) {
    LoggerImpl::log("warning", msg);
  }
  static void error(const std::string& msg) {
    LoggerImpl::log("error", msg);
  }
};
