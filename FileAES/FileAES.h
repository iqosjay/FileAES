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
		//获取AES文件加解密参数
		static AESParams* FetchParams(char**, const int);
		//AES文件加密
		static int Encrpyt(const String&, const String&, const uint8_t*);
		//AES文件解密
		static int Decrpyt(const String&, const String&, const uint8_t*);
		//Byte转Int
		static uint32_t BytesToInt(uint8_t*);
		//Int转Byte
		static void IntToBytes(uint32_t, uint8_t*);
	};
}

