#pragma once

#include "FileAES.h"
#include <io.h>

#define AES_DEFAULT_KEY "IuOWiOgr6uQyLZXL"

//FileAES.exe mode=Encrypt fin=B:\Test.txt fout=B:\TestEncrypt.txt key=IuOWiOgr6uQyLZXL

int main(int argc, char* argv[])
{
	using std::cout;
	using std::endl;
	cout << "�������յ��Ĳ�����" << endl;
	for (int i = 0; i != argc; ++i)
	{
		cout << "argv[" << i << "] = " << argv[i] << endl;
	}
	ChenJie::AESParams* ptr = ChenJie::FileAES::FetchParams(argv, argc);
	if (!ptr)
	{
		cout << "��ȡ����ʧ��" << endl;
		return 1;
	}
	ChenJie::Mode mode = ptr->mode;
	ChenJie::String fin = ptr->srcPath;
	ChenJie::String fout = ptr->outPath;
	uint8_t* key = ptr->key;
	delete (ptr);
	ptr = NULL;
	if (fin.empty())
	{
		cout << "�����ļ���fin��δָ��" << endl;
		return 2;
	}
	if (fout.empty())
	{
		cout << "����ļ���fout��δָ��" << endl;
		return 3;
	}
	const char* outPath = fout.c_str();
	if (0 == _access(outPath, 0))
	{
		if (0 != remove(outPath))
		{
			cout << "Core.jar�Ѿ����ڣ���ɾ��ʧ�ܣ�" << endl;
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
	case ChenJie::Mode::Encrypt:
		code = ChenJie::FileAES::Encrpyt(fin, fout, key);
		break;
	case ChenJie::Mode::Decrypt:
		code = ChenJie::FileAES::Decrpyt(fin, fout, key);
		break;
	case ChenJie::Mode::Unspecified:
	default:
		cout << "modeδָ��" << endl;
		code = 2;
	}
	if (0 == code)
	{
		cout << (mode == ChenJie::Mode::Encrypt ? "������ɣ�" : "") << endl;
	}
	return code;
}