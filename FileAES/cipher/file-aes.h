#ifndef FILE_AES_CIPHER_FILE_AES_H_
#define FILE_AES_CIPHER_FILE_AES_H_

#include "aes.h"
#include "definitions.h"

namespace roy {

void Encrypt(const char* file_in, const char* file_out, uint8* key);
void Decrypt(const char* file_in, const char* file_out, uint8* key);
uint32 BytesToInt(const uint8* bytes);
void IntToBytes(uint32 n, uint8* bytes);

}

#endif//FILE_AES_CIPHER_FILE_AES_H_