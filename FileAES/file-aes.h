#ifndef FILE_AES_WINDOWS_FILE_AES_H_
#define FILE_AES_WINDOWS_FILE_AES_H_

#include <cstdio>

#include "aes.h"
#include "definitions.h"

namespace roy {

// 操作模式
enum class Mode {
  kEncrypt = 0,  // 加密
  kDecrypt,      // 解密
  kUnspecified   // 无效
};

typedef struct AESPARAMS {
  Mode mode_ = Mode::kUnspecified;  // 操作模式
  uint8_t* key_ = 0;                // 加（解）密的 key
  string src_path_ = "";            // 输入文件路径
  string out_path_ = "";            // 输出文件路径
  ~AESPARAMS() { SAFE_DELETE_BUF(key_); }
} AESParams;

class FileAES {
 public:
  // 获取AES文件加解密参数
  static AESParams* FetchParams(int argc, char** argv);

  // 加密文件
  // infile  : 输入文件
  // outfile : 输出文件
  // key     : 加密的 key
  // return  : 当且仅当返回0的时候表示文件加密操作成功
  static int EncryptFile(const char* infile, const char* outfile,
                         const uint8_t* key);

  // 解密文件
  // infile  : 输入文件
  // outfile : 输出文件
  // key     : 解密的 key
  // return  : 当且仅当返回0的时候表示文件解密操作成功
  static int DecryptFile(const char* infile, const char* outfile,
                         const uint8_t* key);

 private:
  FileAES();
  ~FileAES();

 private:
  static uint32_t Byte2ToInt(uint8_t*);
  static void IntToBytes(uint32_t, uint8_t*);
};
}  // namespace roy

#endif  // FILE_AES_WINDOWS_FILE_AES_H_

// TODO: 在此处引用程序需要的其他标头。
