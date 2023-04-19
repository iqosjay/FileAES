#include "file-aes.h"

#include <cstdio>

namespace roy {

void Encrypt(const char* file_in, const char* file_out, uint8* key) {
  struct AES_ctx ctx{};
  AES_init_ctx(&ctx, key);

  // Open File
  FILE* fd = fopen(file_in, "re");
  FILE* fd2 = fopen(file_out, "we");

  uint8 buffer[16];
  size_t size = 0;

  uint8 header[] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
  uint32 line = 0;
  uint32 offset = 0;
  uint8 tempBytes[4];

  // Save Header
  fwrite(header, sizeof(uint8), 16, fd2);

  // Save Body
  while ((size = fread(buffer, sizeof(uint8), 16, fd)) > 0) {
    ++line;
    if (size < 16) {
      offset = (uint32) (16 - size);
      for (size_t index = 16 - offset; index < 16; index++) {
        buffer[index] = '\0';
      }
    }
    AES_ECB_encrypt(&ctx, buffer);
    fwrite(buffer, sizeof(uint8), 16, fd2);
  }

  // Update Header
  IntToBytes(line, tempBytes);
  for (size_t i = 0; i < 4; i++) {
    header[i] = tempBytes[i];
  }
  IntToBytes(offset, tempBytes);
  for (size_t i = 0; i < 4; i++) {
    header[4 + i] = tempBytes[i];
  }
  AES_ECB_encrypt(&ctx, header);
  fseek(fd2, 0, SEEK_SET);
  fwrite(header, sizeof(uint8), 16, fd2);

  // Close File
  fclose(fd);
  fclose(fd2);
}

void Decrypt(const char* file_in, const char* file_out, uint8* key) {
  struct AES_ctx ctx{};
  AES_init_ctx(&ctx, key);

  // Open File
  FILE* fd = fopen(file_in, "re");
  FILE* fd2 = fopen(file_out, "we");

  uint8 buffer[16];
  uint32 lines = 0;

  uint32 line = 0;
  uint32 offset = 0;
  uint8 temp_bytes[4];

  // Parse Header
  if (fread(buffer, sizeof(uint8), 16, fd)) {
    AES_ECB_decrypt(&ctx, buffer);
  }
  for (size_t i = 0; i < 4; i++) {
    temp_bytes[i] = buffer[i];
  }
  line = BytesToInt(temp_bytes);
  for (size_t i = 0; i < 4; i++) {
    temp_bytes[i] = buffer[4 + i];
  }
  offset = BytesToInt(temp_bytes);

  // Parse Body
  while (fread(buffer, sizeof(uint8), 16, fd)) {
    ++lines;
    AES_ECB_decrypt(&ctx, buffer);
    if (line == lines) {
      fwrite(buffer, sizeof(uint8), 16 - offset, fd2);
    } else {
      fwrite(buffer, sizeof(uint8), 16, fd2);
    }
  }

  // Close File
  fclose(fd);
  fclose(fd2);
}

void IntToBytes(uint32 n, uint8* bytes) {
  bytes[0] = (uint8) (0xFF & n);
  bytes[1] = (uint8) ((0xFF00 & n) >> 8);
  bytes[2] = (uint8) ((0xFF0000 & n) >> 16);
  bytes[3] = (uint8) ((0xFF000000 & n) >> 24);
}

uint32 BytesToInt(const uint8* bytes) {
  uint32 num = bytes[0] & 0xFF;
  num |= ((bytes[1] << 8) & 0xFF00);
  num |= ((bytes[2] << 16) & 0xFF0000);
  num |= ((bytes[3] << 24) & 0xFF000000);
  return num;
}

}