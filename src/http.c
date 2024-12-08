#include <windows.h>
#include <winhttp.h>
#include <stdlib.h>

#pragma comment(lib, "winhttp.lib")

// For future: Implement custom GetModuleHandle and GetProcAddress to dynamically resolve these WinHTTP functions
char* download_file(const wchar_t* host, const wchar_t* path, int port, size_t* totalSize) {
    HINTERNET hSession = WinHttpOpen(L"WinHTTP Example/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession) return NULL;

    HINTERNET hConnect = WinHttpConnect(hSession, host, port, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return NULL;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path,
        NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return NULL;
    }

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(hRequest, NULL)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return NULL;
    }

    DWORD bytesRead = 0;
    DWORD bufferSize = 0;
    char* buffer = NULL;
    char tempBuffer[4096];
    *totalSize = 0;

    do {
        if (WinHttpQueryDataAvailable(hRequest, &bufferSize) && bufferSize > 0) {
            char* newBuffer = (char*)realloc(buffer, *totalSize + bufferSize);
            if (!newBuffer) {
                free(buffer);
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return NULL;
            }
            buffer = newBuffer;

            if (WinHttpReadData(hRequest, tempBuffer, bufferSize, &bytesRead)) {
                if (bytesRead > 0) {
                    memcpy(buffer + *totalSize, tempBuffer, bytesRead);
                    *totalSize += bytesRead;
                } else {
                    break;
                }
            } else {
                break;
            }
        } else {
            break;
        }
    } while (bytesRead > 0);

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return buffer;
}
