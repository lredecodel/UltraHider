#include "encryptor.h"
#include <stdio.h>
#include <stdlib.h>

// For future: add support for multi-byte key
void xor_encrypt_decrypt(char* buffer, size_t size, int key) {
    for (size_t i = 0; i < size - 1; i++) {
        buffer[i] ^= key;
    }
}