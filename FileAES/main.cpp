#include <io.h>
#include "FileAes.h"
#define AES_DEFAULT_KEY "ABCDEFGHIJKLMNOP"
int main(int argc, char* argv[])
{
	using std::cout;
	using std::endl;
	cout << "�������յ��Ĳ�����" << endl;
	for (int i = 0; i != argc; ++i)
	{
		cout << "argv[" << i << "] = " << argv[i] << endl;
	}
	Roy::AESParams* ptr = Roy::FileAES::FetchParams(argv, argc);
	if (!ptr)
	{
		cout << "��ȡ����ʧ�ܣ�" << endl;
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
		cout << "�����ļ���fin��δָ����" << endl;
		return 2;
	}
	if (fout.empty())
	{
		cout << "����ļ���fout��δָ����" << endl;
		return 3;
	}
	const char* outPath = fout.c_str();
	if (0 == _access(outPath, 0))
	{
		if (0 != remove(outPath))
		{
			cout << "����ļ�����ͬ���ļ�����ɾ��ʧ�ܣ�" << endl;
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
		cout << "Modeδָ����" << endl;
		code = 2;
	}
	if (0 == code)
	{
		cout << ((mode == Roy::Mode::Encrypt) ? "������ɣ�" : (mode == Roy::Mode::Decrypt) ? "������ɣ�" : "") << endl;
	}
	return code;
}