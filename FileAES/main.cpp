#include <io.h>
#include "FileAes.h"
#define AES_DEFAULT_KEY "ABCDEFGHIJKLMNOP"
int main(int argc, char* argv[])
{
	using std::cout;
	using std::endl;
	cout << "主函数收到的参数：" << endl;
	for (int i = 0; i != argc; ++i)
	{
		cout << "argv[" << i << "] = " << argv[i] << endl;
	}
	Roy::AESParams* ptr = Roy::FileAES::FetchParams(argv, argc);
	if (!ptr)
	{
		cout << "获取参数失败！" << endl;
		return 1;
	}
	Roy::Mode mode = ptr->mode;
	Roy::String fin = ptr->srcPath;
	Roy::String fout = ptr->outPath;
	uint8_t* key = ptr->key;
	delete (ptr);
	ptr = NULL;
	if (fin.empty())
	{
		cout << "输入文件（fin）未指定！" << endl;
		return 2;
	}
	if (fout.empty())
	{
		cout << "输出文件（fout）未指定！" << endl;
		return 3;
	}
	const char* outPath = fout.c_str();
	if (0 == _access(outPath, 0))
	{
		if (0 != remove(outPath))
		{
			cout << "输出文件存在同名文件！且删除失败！" << endl;
			return -1;
		}
	}
	if (0 == key)
	{
		key = (uint8_t*)AES_DEFAULT_KEY;
	}
	int code;
	switch (mode)
	{
	case Roy::Mode::Encrypt:
		code = Roy::FileAES::EncryptFile(fin.c_str(), fout.c_str(), key);
		break;
	case Roy::Mode::Decrypt:
		code = Roy::FileAES::DecryptFile(fin.c_str(), fout.c_str(), key);
		break;
	case Roy::Mode::Unspecified:
	default:
		cout << "Mode未指定！" << endl;
		code = 2;
	}
	if (0 == code)
	{
		cout << ((mode == Roy::Mode::Encrypt) ? "加密完成！" : (mode == Roy::Mode::Decrypt) ? "解密完成！" : "") << endl;
	}
	return code;
}