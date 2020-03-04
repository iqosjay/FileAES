// FileAes.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <cstdio>
#include "RoyType.h"
#include "Aes.h"

namespace Roy {

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

	class FileAES {
	private:
		FileAES() = default;
		~FileAES() = default;
	private:
		static uint32_t Byte2ToInt(uint8_t*);
		static void IntToBytes(uint32_t, uint8_t*);
	public:
		//获取AES文件加解密参数
		static AESParams* FetchParams(char**, const int);
		static int EncryptFile(const char*, const char*, const uint8_t*);
		static int DecryptFile(const char*, const char*, const uint8_t*);
	};
}

// TODO: 在此处引用程序需要的其他标头。
