#include "FileAes.h"

namespace Roy {
	/*
	获取AES文件加解密参数
	@param argv		主函数中的参数（cmd中传递过来的）
	@param len		数组的长度
	*/
	AESParams* FileAES::FetchParams(char** argv, const int len)
	{
		AESParams* ptrParam = new AESParams();
		for (int i = 0; i != len; ++i)
		{
			String tmpValue = argv[i];
			String::size_type pos = 0;
			if (-1 != (pos = tmpValue.find("mode=")))
			{
				String mode = tmpValue.substr(pos + 5);
				if ("Encrypt" == mode)
				{
					ptrParam->mode = Mode::Encrypt;
				}
				else if ("Decrypt" == mode)
				{
					ptrParam->mode = Mode::Decrypt;
				}
				else
				{
					ptrParam->mode = Mode::Unspecified;
					break;
				}
			}
			else if (-1 != (pos = tmpValue.find("fin=")))
			{
				ptrParam->srcPath = tmpValue.substr(pos + 4);
			}
			else if (-1 != (pos = tmpValue.find("fout=")))
			{
				ptrParam->outPath = tmpValue.substr(pos + 5);
			}
			else if (-1 != (pos = tmpValue.find("key=")))
			{
				const char* key = tmpValue.substr(pos + 4).c_str();
				ptrParam->key = (uint8_t*)key;
			}
		}
		return ptrParam;
	}
	//AES文件加密
	//inputFile  是待加密的文件路径
	//outputFile 是加密成功之后输出文件的路径
	//返回值为一个整形 当且仅当返回0的时候表示文件加密操作成功
	int FileAES::EncryptFile(const char* inputFile, const char* outputFile, const uint8_t* key)
	{
		struct AesCtx ctx;
		Aes::InitAesCtx(&ctx, key);
		//Check Whether The File Is Exists
		// Open File
		FILE* fd = fopen(inputFile, "rb");
		FILE* fd2 = fopen(outputFile, "wb+");

		uint8_t buffer[16];
		size_t size = 0;

		uint8_t header[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
							0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
		uint32_t line = 0;
		uint32_t offset = 0;
		uint8_t tempBytes[4];

		// Save Header
		fwrite(header, sizeof(uint8_t), 16, fd2);

		// Save Body
		while ((size = fread(buffer, sizeof(uint8_t), 16, fd)) > 0) {
			line++;
			if (size < 16) {
				offset = (uint32_t)(16 - size);
				for (size_t index = 16 - offset; index < 16; index++) {
					buffer[index] = '\0';
				}
			}
			Aes::AesECBEncrypt(&ctx, buffer);
			fwrite(buffer, sizeof(uint8_t), 16, fd2);
		}

		// Update Header
		FileAES::IntToBytes(line, tempBytes);
		for (size_t i = 0; i < 4; i++) {
			header[i] = tempBytes[i];
		}
		FileAES::IntToBytes(offset, tempBytes);
		for (size_t i = 0; i < 4; i++) {
			header[4 + i] = tempBytes[i];
		}
		Aes::AesECBEncrypt(&ctx, header);
		fseek(fd2, 0, SEEK_SET);
		fwrite(header, sizeof(uint8_t), 16, fd2);

		// Close File
		fclose(fd);
		fclose(fd2);
		return 0;
	}

	//AES文件解密
	//inputFile  是待解密的文件路径
	//outputFile 是解密成功之后输出文件的路径
	//返回值为一个整形 当且仅当返回0的时候表示文件解密操作成功
	int FileAES::DecryptFile(const char* inputFile, const char* outputFile, const uint8_t* key)
	{
		struct AesCtx ctx;
		Aes::InitAesCtx(&ctx, key);

		// Open File
		FILE* fd = fopen(inputFile, "rb");
		FILE* fd2 = fopen(outputFile, "wb+");

		uint8_t buffer[16];
		uint32_t lines = 0;

		uint32_t line = 0;
		uint32_t offset = 0;
		uint8_t tempBytes[4];

		// Parse Header
		if (fread(buffer, sizeof(uint8_t), 16, fd)) {
			Aes::AesECBDecrypt(&ctx, buffer);
		}
		for (size_t i = 0; i < 4; i++) {
			tempBytes[i] = buffer[i];
		}
		line = FileAES::Byte2ToInt(tempBytes);
		for (size_t i = 0; i < 4; i++) {
			tempBytes[i] = buffer[4 + i];
		}
		offset = FileAES::Byte2ToInt(tempBytes);

		// Parse Body
		while (fread(buffer, sizeof(uint8_t), 16, fd)) {
			lines++;
			Aes::AesECBDecrypt(&ctx, buffer);
			if (line == lines) {
				fwrite(buffer, sizeof(uint8_t), 16 - offset, fd2);
			}
			else {
				fwrite(buffer, sizeof(uint8_t), 16, fd2);
			}
		}

		// Close File
		fclose(fd);
		fclose(fd2);
		return 0;
	}

	uint32_t FileAES::Byte2ToInt(uint8_t* bytes)
	{
		uint32_t iRetVal = bytes[0] & 0xFF;
		iRetVal |= ((bytes[1] << 8) & 0xFF00);
		iRetVal |= ((bytes[2] << 16) & 0xFF0000);
		iRetVal |= ((bytes[3] << 24) & 0xFF000000);
		return iRetVal;
	}

	void FileAES::IntToBytes(uint32_t i, uint8_t* bytes)
	{
		bytes[0] = (uint8_t)(0xFF & i);
		bytes[1] = (uint8_t)((0xFF00 & i) >> 8);
		bytes[2] = (uint8_t)((0xFF0000 & i) >> 16);
		bytes[3] = (uint8_t)((0xFF000000 & i) >> 24);
	}
}

