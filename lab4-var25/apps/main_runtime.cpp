#include <iostream>
#include <memory>
#include <string>

#include "platform/dynlib.h"

using gcf_func_t = int (*)(int, int);
using translation_func_t = char *(*)(long long);

int main() {
  std::string gcf_path = "./libgcf_euclid.so";
  std::string trans_path = "./libtranslation_binary.so";

  std::unique_ptr<platform::DynamicLibrary> gcf_lib;
  std::unique_ptr<platform::DynamicLibrary> trans_lib;

  try {
    gcf_lib = std::make_unique<platform::DynamicLibrary>(gcf_path);
    trans_lib = std::make_unique<platform::DynamicLibrary>(trans_path);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  auto gcf_func = gcf_lib->get_function<gcf_func_t>("gcf");
  auto trans_func = trans_lib->get_function<translation_func_t>("translation");

  std::cout << "Euclid gcf and binary translation (runtime)\n"
            << "Select mode:\n\t0 - switch libraries\n"
            << "\t1 - GCF\n\t2 - translation\n";
  std::string cmd;
  while (std::cin >> cmd) {
    if (cmd == "0") {
      if (gcf_path == "./libgcf_euclid.so") {
        gcf_path = "./libgcf_naive.so";
        trans_path = "./libtranslation_ternary.so";
      } else {
        gcf_path = "./libgcf_euclid.so";
        trans_path = "./libtranslation_binary.so";
      }

      try {
        gcf_lib = std::make_unique<platform::DynamicLibrary>(gcf_path);
        trans_lib = std::make_unique<platform::DynamicLibrary>(trans_path);

        gcf_func = gcf_lib->get_function<gcf_func_t>("gcf");
        trans_func = trans_lib->get_function<translation_func_t>("translation");
        std::cout << "Switched to alternative.\n";
      } catch (const std::exception &e) {
        std::cerr << "Switch failed: " << e.what() << "\n";
      }
    } else if (cmd == "1") {
      std::cout << "Write: A B\n";
      int a;
      int b;
      std::cin >> a >> b;
      std::cout << "GCF(" << a << ", " << b << ") = " << gcf_func(a, b) << "\n";
    } else if (cmd == "2") {
      std::cout << "Write: X\n";
      long long x;
      std::cin >> x;
      char *res = trans_func(x);
      std::cout << "Translation(" << x << ") = " << res << "\n";
      std::free(res);
    }
  }

  return 0;
}