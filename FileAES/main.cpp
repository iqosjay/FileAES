#include <io.h>

#include <iostream>

#include "file-aes.h"

#define AES_DEFAULT_KEY "ABCDEFGHIJKLMNOP"

int main(int argc, char* argv[]) {
  using std::cout;
  using std::endl;
  cout << "主函数收到的参数：" << endl;
  for (int i = 0; i != argc; ++i) {
    cout << "argv[" << i << "] = " << argv[i] << endl;
  }
  auto params = roy::FileAES::FetchParams(argc, argv);
  if (nullptr == params) {
    cout << "获取参数失败！" << endl;
    return 1;
  }
  roy::Mode mode = params->mode_;
  roy::string fin = params->src_path_;
  roy::string fout = params->out_path_;
  uint8_t* key = params->key_;
  SAFE_DELETE_PTR(params);
  if (fin.empty()) {
    cout << "输入文件（fin）未指定！" << endl;
    return 2;
  }
  if (fout.empty()) {
    cout << "输出文件（fout）未指定！" << endl;
    return 3;
  }
  const char* out_path = fout.c_str();
  if (0 == _access(out_path, 0)) {
    if (0 != remove(out_path)) {
      cout << "输出文件存在同名文件！且删除失败！" << endl;
      return -1;
    }
  }
  if (0 == key) {
    key = (uint8_t*)AES_DEFAULT_KEY;
  }
  int code;
  switch (mode) {
    case roy::Mode::kEncrypt:
      code = roy::FileAES::EncryptFile(fin.c_str(), fout.c_str(), key);
      break;
    case roy::Mode::kDecrypt:
      code = roy::FileAES::DecryptFile(fin.c_str(), fout.c_str(), key);
      break;
    case roy::Mode::kUnspecified:
    default:
      cout << "Mode未指定！" << endl;
      code = 2;
  }
  if (0 == code) {
    cout << ((mode == roy::Mode::kEncrypt)   ? "加密完成！"
             : (mode == roy::Mode::kDecrypt) ? "解密完成！"
                                             : "")
         << endl;
  }
  return code;
}