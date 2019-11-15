#pragma once

#include <iostream>
#include "AES.h"

namespace ChenJie {
	typedef std::string String;

	enum class Mode {
		Encrypt = 0,
		Decrypt,
		Unspecified
	};

	struct AESParams
	{
		Mode mode = Mode::Unspecified;
		uint8_t* key = 0;
		String srcPath = "";
		String outPath = "";
	};

	class FileAES
	{
	public:
		//��ȡAES�ļ��ӽ��ܲ���
		static AESParams* FetchParams(char**, const int);
		//AES�ļ�����
		static int Encrpyt(const String&, const String&, const uint8_t*);
		//AES�ļ�����
		static int Decrpyt(const String&, const String&, const uint8_t*);
		//ByteתInt
		static uint32_t BytesToInt(uint8_t*);
		//IntתByte
		static void IntToBytes(uint32_t, uint8_t*);
	};
}

