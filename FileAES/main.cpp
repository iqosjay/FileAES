#include <iostream>

#include "cipher/file-aes.h"

namespace {
constexpr const char* kUsage =
    "Usage :\n"
    "      ./fileaes --mode=<MODE> --fin=<PATH1> --fout=<PATH2> --key=<KEY>\n"
    "      <MODE>  : encrypt or decrypt\n"
    "      <PATH1> : the path of input file\n"
    "      <PATH2> : the path of output file\n"
    "      <KEY>   : the key that used for encryption or decryption\n"
    "Eg    :\n"
    "      ./fileaes --mode=encrypt --fin=a.png --fout=b.png --key=9i8zUx02h8Cz5Roy";
}

inline void print_usage() {
  std::cout << kUsage << std::endl;
}

int main(int argc, char** argv) {
  if (5 != argc) {
    print_usage();
    return 0;
  }
  std::string mode, file_in, file_out, key;
  for (int i = 0; i != argc; ++i) {
    const std::string pair = argv[i];
    const std::string::size_type pos = pair.find('=');
    if (std::string::npos == pos) {
      continue;
    }
    const std::string k = pair.substr(0, pos);
    const std::string v = pair.substr(pos + 1);
    if ("--mode" == k) {
      mode = v;
    } else if ("--fin" == k) {
      file_in = v;
    } else if ("--fout" == k) {
      file_out = v;
    } else if ("--key" == k) {
      key = v;
    }
  }
  if (file_in.empty() || file_out.empty() || key.empty()) {
    print_usage();
    return 0;
  }
  if ("encrypt" == mode) {
    roy::Encrypt(file_in.c_str(), file_out.c_str(), (roy::uint8*) key.c_str());
  } else if ("decrypt" == mode) {
    roy::Decrypt(file_in.c_str(), file_out.c_str(), (roy::uint8*) key.c_str());
  } else {
    print_usage();
  }
  return 0;
}