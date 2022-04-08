#include "file-aes.h"

namespace roy {

FileAES::FileAES() {}

FileAES::~FileAES() {}

AESParams* FileAES::FetchParams(int argc, char** argv) {
  AESParams* params = new AESParams();
  for (int i = 0; i != argc; ++i) {
    string tmp_value = argv[i];
    string::size_type pos = 0;
    if (-1 != (pos = tmp_value.find("mode="))) {
      string mode = tmp_value.substr(pos + 5);
      if ("Encrypt" == mode) {
        params->mode_ = Mode::kEncrypt;
      } else if ("Decrypt" == mode) {
        params->mode_ = Mode::kDecrypt;
      } else {
        params->mode_ = Mode::kUnspecified;
        break;
      }
    } else if (-1 != (pos = tmp_value.find("fin="))) {
      params->src_path_ = tmp_value.substr(pos + 4);
    } else if (-1 != (pos = tmp_value.find("fout="))) {
      params->out_path_ = tmp_value.substr(pos + 5);
    } else if (-1 != (pos = tmp_value.find("key="))) {
      string key_str = tmp_value.substr(pos + 4);
      params->key_ = new uint8_t[key_str.size()];
      memset(params->key_, 0, key_str.size());
      memcpy(params->key_, key_str.c_str(), key_str.size());
    }
  }
  return params;
}

int FileAES::EncryptFile(const char* infile, const char* outfile,
                         const uint8_t* key) {
  struct AesCtx ctx;
  Aes::InitAesCtx(&ctx, key);
  // Check Whether The File Is Exists
  //  Open File
  FILE* fd = fopen(infile, "rb");
  FILE* fd2 = fopen(outfile, "wb+");

  uint8_t buffer[16];
  size_t size = 0;

  uint8_t header[] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                      0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
  uint32_t line = 0;
  uint32_t offset = 0;
  uint8_t temp_bytes[4];

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
  FileAES::IntToBytes(line, temp_bytes);
  for (size_t i = 0; i < 4; i++) {
    header[i] = temp_bytes[i];
  }
  FileAES::IntToBytes(offset, temp_bytes);
  for (size_t i = 0; i < 4; i++) {
    header[4 + i] = temp_bytes[i];
  }
  Aes::AesECBEncrypt(&ctx, header);
  fseek(fd2, 0, SEEK_SET);
  fwrite(header, sizeof(uint8_t), 16, fd2);

  // Close File
  fclose(fd);
  fclose(fd2);
  return 0;
}

int FileAES::DecryptFile(const char* infile, const char* outfile,
                         const uint8_t* key) {
  struct AesCtx ctx;
  Aes::InitAesCtx(&ctx, key);

  // Open File
  FILE* fd = fopen(infile, "rb");
  FILE* fd2 = fopen(outfile, "wb+");

  uint8_t buffer[16];
  uint32_t lines = 0;

  uint32_t line = 0;
  uint32_t offset = 0;
  uint8_t temp_bytes[4];

  // Parse Header
  if (fread(buffer, sizeof(uint8_t), 16, fd)) {
    Aes::AesECBDecrypt(&ctx, buffer);
  }
  for (size_t i = 0; i < 4; i++) {
    temp_bytes[i] = buffer[i];
  }
  line = FileAES::Byte2ToInt(temp_bytes);
  for (size_t i = 0; i < 4; i++) {
    temp_bytes[i] = buffer[4 + i];
  }
  offset = FileAES::Byte2ToInt(temp_bytes);

  // Parse Body
  while (fread(buffer, sizeof(uint8_t), 16, fd)) {
    Aes::AesECBDecrypt(&ctx, buffer);
    if (line == ++lines) {
      fwrite(buffer, sizeof(uint8_t), 16 - offset, fd2);
    } else {
      fwrite(buffer, sizeof(uint8_t), 16, fd2);
    }
  }

  // Close File
  fclose(fd);
  fclose(fd2);
  return 0;
}

uint32_t FileAES::Byte2ToInt(uint8_t* bytes) {
  uint32_t iRetVal = bytes[0] & 0xFF;
  iRetVal |= ((bytes[1] << 8) & 0xFF00);
  iRetVal |= ((bytes[2] << 16) & 0xFF0000);
  iRetVal |= ((bytes[3] << 24) & 0xFF000000);
  return iRetVal;
}

void FileAES::IntToBytes(uint32_t i, uint8_t* bytes) {
  bytes[0] = (uint8_t)(0xFF & i);
  bytes[1] = (uint8_t)((0xFF00 & i) >> 8);
  bytes[2] = (uint8_t)((0xFF0000 & i) >> 16);
  bytes[3] = (uint8_t)((0xFF000000 & i) >> 24);
}
}  // namespace roy
