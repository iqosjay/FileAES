#include "FileAES.h"
#include <fstream>

namespace ChenJie {
	typedef std::ifstream InputFile;
	typedef std::ofstream OutputFile;
	
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

	/*
	AES文件加密
	@param src		要加密的文件的路径
	@param outPath	加密过后文件的输出路径
	@param key		加密文件所使用的Key
	*/
	int FileAES::Encrpyt(const String& input, const String& output, const uint8_t* key)
	{
		using std::ios;
		struct AES_ctx ctx;
		AES_init_ctx(&ctx, key);
		// Open File
		InputFile fin;
		OutputFile fout;
		fin.open(input.c_str(), ios::in | ios::binary);
		if (!fin.is_open())
		{
			std::cout << "输入文件不存在！" << std::endl;
			return -1;
		}
		fout.open(output.c_str(), ios::out | ios::binary | ios::trunc);
		if (!fout.is_open())
		{
			std::cout << "无法创建输出文件！" << std::endl;
			return -1;
		}

		uint8_t buffer[16];

		uint8_t header[] = {
			0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
			0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
		};
		uint32_t line = 0;
		uint32_t offset = 0;
		uint8_t tempBytes[4];

		// Save Header
		fout.write((char*)header, 16 * sizeof(uint8_t));

		// Save Body
		while (fin.read((char*)buffer, sizeof(uint8_t) * 16))
		{
			++line;
			size_t size = fin.gcount();
			if (size < 16)
			{
				offset = (uint32_t)16 - size;
				for (size_t index = 16 - offset; index != 16; ++index)
				{
					buffer[index] = '\0';
				}
			}
			AES_ECB_encrypt(&ctx, buffer);
			fout.write((char*)buffer, 16 * sizeof(uint8_t));
		}

		// Update Header
		IntToBytes(line, tempBytes);
		for (size_t i = 0; i != 4; ++i)
		{
			header[i] = tempBytes[i];
		}
		IntToBytes(offset, tempBytes);
		for (size_t i = 0; i != 4; ++i)
		{
			header[4 + i] = tempBytes[i];
		}
		AES_ECB_encrypt(&ctx, header);
		fout.seekp(0, ios::beg);
		fout.write((char*)header, 16 * sizeof(uint8_t));

		// Close File
		fout.close();
		fin.close();
		return 0;
	}
	/*
	AES文件解密
	@param src		要解密的文件的路径
	@param outPath	解密过后文件的输出路径
	@param key		解密文件所使用的Key
	*/
	int FileAES::Decrpyt(const String& input, const String& output, const uint8_t* key)
	{
		using std::cout;
		using std::endl;
		using std::ios;

		struct AES_ctx ctx;
		AES_init_ctx(&ctx, key);

		InputFile fin;
		OutputFile fout;

		// Open File
		fin.open(input.c_str(), ios::in | ios::binary);
		if (!fin.is_open())
		{
			cout << "输入文件不存在！" << endl;
			return -1;
		}

		fout.open(output.c_str(), ios::out | ios::binary | ios::trunc);
		if (!fout.is_open())
		{
			cout << "无法创建输出文件！" << endl;
			return -1;
		}

		uint8_t buffer[16];
		uint8_t header[] = {
			0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
			0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
		};
		uint8_t tempBytes[4];

		// Parse Header
		if (fin.read((char*)buffer, 16 * sizeof(uint8_t)))
		{
			AES_ECB_decrypt(&ctx, buffer);
		}

		for (size_t i = 0; i != 4; ++i)
		{
			tempBytes[i] = buffer[i];
		}

		uint32_t line = BytesToInt(tempBytes);
		for (size_t i = 0; i != 4; ++i)
		{
			tempBytes[i] = buffer[4 + i];
		}
		uint32_t offset = BytesToInt(tempBytes);

		// Parse Body
		uint32_t lines = 0;
		while (fin.read((char*)buffer, 16 * sizeof(uint8_t)))
		{
			AES_ECB_decrypt(&ctx, buffer);
			if (line == ++lines)
			{
				fout.write((char*)buffer, (16 - offset) * sizeof(uint8_t));
			}
			else
			{
				fout.write((char*)buffer, 16 * sizeof(uint8_t));
			}
		}
		// Close File
		fout.close();
		fin.close();
		return 0;
	}
	/*
	Byte数组转Int
	@param bytes	要转化的Byte数组
	@return			转化之后的结果
	*/
	uint32_t FileAES::BytesToInt(uint8_t* bytes)
	{
		uint32_t iRetVal = bytes[0] & 0xFF;
		iRetVal |= ((bytes[1] << 8) & 0xFF00);
		iRetVal |= ((bytes[2] << 16) & 0xFF0000);
		iRetVal |= ((bytes[3] << 24) & 0xFF000000);
		return iRetVal;
	}
	/*
	Int转为Byte数组
	@param i		要转化的Int
	@param bytes	转化完成之后用于存放结果的Byte数组
	*/
	void FileAES::IntToBytes(uint32_t i, uint8_t* bytes)
	{
		bytes[0] = (uint8_t)(0xFF & i);
		bytes[1] = (uint8_t)((0xFF00 & i) >> 8);
		bytes[2] = (uint8_t)((0xFF0000 & i) >> 16);
		bytes[3] = (uint8_t)((0xFF000000 & i) >> 24);
	}
}