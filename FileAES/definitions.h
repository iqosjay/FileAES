#ifndef FILE_AES_WINDOWS_DEFINITIONS_H_
#define FILE_AES_WINDOWS_DEFINITIONS_H_

#include <string>

#define SAFE_DELETE_PTR(PTR) \
  do {                       \
    if (PTR) {               \
      delete (PTR);          \
      PTR = nullptr;         \
    }                        \
  } while (0)

#define SAFE_DELETE_BUF(BUF) \
  do {                       \
    if (BUF) {               \
      delete[](BUF);         \
      BUF = nullptr;         \
    }                        \
  } while (0)

namespace roy {

using string = std::string;

typedef uint8_t state_t[4][4];

}  // namespace roy

#endif  // FILE_AES_WINDOWS_DEFINITIONS_H_
