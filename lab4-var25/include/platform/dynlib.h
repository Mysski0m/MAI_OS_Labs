#pragma once

#include <memory>
#include <string>

namespace platform {

class DynamicLibrary {
public:
  explicit DynamicLibrary(const std::string &path);
  ~DynamicLibrary();

  DynamicLibrary(const DynamicLibrary &) = delete;
  DynamicLibrary &operator=(const DynamicLibrary &) = delete;

  template <typename Func> Func get_function(const char *name) const {
    return reinterpret_cast<Func>(get_symbol_impl(name));
  }

private:
  void *get_symbol_impl(const char *name) const;
  void *handle_ = nullptr;
};

} // namespace platform