// Второй файл чтобы через него были вторые реализации, потому что в runtime их
// менять нельзя, только если через CMake

#include <cstdlib>
#include <iostream>
#include <string>

#include "gcf.h"
#include "translation.h"

int main() {
  std::string cmd;
  std::cout << "Naive gcf and ternary translation (linking)\n"
            << "Select mode:\n\t0 - switch libraries (not available in static "
               "linkage mode)\n"
            << "\t1 - GCF\n\t2 - translation\n";
  while (std::cin >> cmd) {
    if (cmd == "0") {
      std::cout << "Switch libraries not available in static linkage.\n";
    } else if (cmd == "1") {
      std::cout << "Write: A B\n";
      int a;
      int b;
      std::cin >> a >> b;
      std::cout << "GCF(" << a << ", " << b << ") = " << gcf(a, b) << "\n";
    } else if (cmd == "2") {
      std::cout << "Write: X\n";
      long long x;
      std::cin >> x;
      char *res = translation(x);
      std::cout << "Translation(" << x << ") = " << res << "\n";
      std::free(res);
    }
  }
  return 0;
}