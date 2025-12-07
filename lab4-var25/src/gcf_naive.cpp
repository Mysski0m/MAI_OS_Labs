#include <algorithm>

#include "gcf.h"

int gcf(int a, int b) {
  int min_value = std::min(a, b);
  for (int i = min_value; i >= 1; --i) {
    if (a % i == 0 && b % i == 0)
      return i;
  }
  return 1;
}