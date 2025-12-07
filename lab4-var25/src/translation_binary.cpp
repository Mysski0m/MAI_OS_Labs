#include <cstdlib>
#include <cstring>

#include "translation.h"

const int8_t BUFFER = 65;

char *translation(long long x) {
  if (x == 0) {
    char *res = static_cast<char *>(malloc(2));
    std::strcpy(res, "0");
    return res;
  }

  bool neg = x < 0;
  if (neg)
    x = -x;

  char buffer[BUFFER];
  int i = 0;
  while (x > 0) {
    buffer[i++] = '0' + (x & 1);
    x >>= 1;
  }
  if (neg)
    buffer[i++] = '-';

  char *result = static_cast<char *>(malloc(i + 1));
  for (int j = 0; j < i; ++j) {
    result[j] = buffer[i - 1 - j];
  }
  result[i] = '\0';
  return result;
}