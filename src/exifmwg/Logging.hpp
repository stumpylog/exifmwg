#ifdef NANOBIND_MODULE
#include <nanobind/nanobind.h>
namespace nb = nanobind;
#endif

#include <cstring>
#include <iostream>

constexpr const char* _moduleName = "exifmwg.bindings";

#ifdef NANOBIND_MODULE
struct PythonLogger {
  static auto get_logger() {
    static nb::object logger;
    static bool initialized = false;
    if (!initialized) {
      auto logging = nb::module_::import_("logging");
      logger = logging.attr("getLogger")(_moduleName);
      initialized = true;
    }
    return logger;
  }

  template <typename T> static void log(const char* level, const T& msg) {
    get_logger().attr(level)(msg);
  }
};
#else
struct NativeLogger {
  template <typename T> static void log(const char* level, const T& msg) {
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
