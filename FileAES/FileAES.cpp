#include "FileAES.h"
#include <fstream>

namespace ChenJie {
	typedef std::ifstream InputFile;
	typedef std::ofstream OutputFile;
	
	/*
	��ȡAES�ļ��ӽ��ܲ���
	@param argv		�������еĲ�����cmd�д��ݹ����ģ�
	@param len		����ĳ���
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
	AES�ļ�����
	@param src		Ҫ���ܵ��ļ���·��
	@param outPath	���ܹ����ļ������·��
	@param key		�����ļ���ʹ�õ�Key
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
			std::cout << "�����ļ������ڣ�" << std::endl;
			return -1;
		}
		fout.open(output.c_str(), ios::out | ios::binary | ios::trunc);
		if (!fout.is_open())
		{
			std::cout << "�޷���������ļ���" << std::endl;
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
	AES�ļ�����
	@param src		Ҫ���ܵ��ļ���·��
	@param outPath	���ܹ����ļ������·��
	@param key		�����ļ���ʹ�õ�Key
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
			cout << "�����ļ������ڣ�" << endl;
			return -1;
		}

		fout.open(output.c_str(), ios::out | ios::binary | ios::trunc);
		if (!fout.is_open())
		{
			cout << "�޷���������ļ���" << endl;
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
	Byte����תInt
	@param bytes	Ҫת����Byte����
	@return			ת��֮��Ľ��
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
	IntתΪByte����
	@param i		Ҫת����Int
	@param bytes	ת�����֮�����ڴ�Ž����Byte����
	*/
	void FileAES::IntToBytes(uint32_t i, uint8_t* bytes)
	{
		bytes[0] = (uint8_t)(0xFF & i);
		bytes[1] = (uint8_t)((0xFF00 & i) >> 8);
		bytes[2] = (uint8_t)((0xFF0000 & i) >> 16);
		bytes[3] = (uint8_t)((0xFF000000 & i) >> 24);
	}
}