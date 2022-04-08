#include <io.h>

#include <iostream>

#include "file-aes.h"

#define AES_DEFAULT_KEY "ABCDEFGHIJKLMNOP"

int main(int argc, char* argv[]) {
  using std::cout;
  using std::endl;
  cout << "�������յ��Ĳ�����" << endl;
  for (int i = 0; i != argc; ++i) {
    cout << "argv[" << i << "] = " << argv[i] << endl;
  }
  auto params = roy::FileAES::FetchParams(argc, argv);
  if (nullptr == params) {
    cout << "��ȡ����ʧ�ܣ�" << endl;
    return 1;
  }
  roy::Mode mode = params->mode_;
  roy::string fin = params->src_path_;
  roy::string fout = params->out_path_;
  uint8_t* key = params->key_;
  SAFE_DELETE_PTR(params);
  if (fin.empty()) {
    cout << "�����ļ���fin��δָ����" << endl;
    return 2;
  }
  if (fout.empty()) {
    cout << "����ļ���fout��δָ����" << endl;
    return 3;
  }
  const char* out_path = fout.c_str();
  if (0 == _access(out_path, 0)) {
    if (0 != remove(out_path)) {
      cout << "����ļ�����ͬ���ļ�����ɾ��ʧ�ܣ�" << endl;
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
      cout << "Modeδָ����" << endl;
      code = 2;
  }
  if (0 == code) {
    cout << ((mode == roy::Mode::kEncrypt)   ? "������ɣ�"
             : (mode == roy::Mode::kDecrypt) ? "������ɣ�"
                                             : "")
         << endl;
  }
  return code;
}