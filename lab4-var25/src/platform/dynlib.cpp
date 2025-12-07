#include "platform/dynlib.h"

#include <dlfcn.h>
#include <stdexcept>
#include <string>

namespace platform {

DynamicLibrary::DynamicLibrary(const std::string &path)
    : handle_(dlopen(path.c_str(), RTLD_LAZY)) {
  if (!handle_) {
    throw std::runtime_error("Failed to load library: " + path);
  }
}

DynamicLibrary::~DynamicLibrary() {
  if (handle_) {
    dlclose(handle_);
  }
}

void *DynamicLibrary::get_symbol_impl(const char *name) const {
  void *symbol = dlsym(handle_, name);
  if (!symbol) {
    throw std::runtime_error("Symbol not found: " + std::string(name));
  }
  return symbol;
}

} // namespace platform