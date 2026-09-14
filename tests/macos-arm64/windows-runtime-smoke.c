/* Small Windows guest test for the experimental ARM64 Wine/FEX runtime. */
#include <windows.h>
#include <stdio.h>

static DWORD WINAPI worker(void *argument)
{
    volatile LONG *counter = argument;
    for (unsigned i = 0; i < 10000; ++i) InterlockedIncrement(counter);
    return 0;
}

int main(void)
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    if (info.dwPageSize != 4096) return 10;
    unsigned char *memory = VirtualAlloc(NULL, 8192, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!memory) return 11;
    memory[0] = 42;
    memory[4096] = 43;
    DWORD old;
    if (!VirtualProtect(memory, 4096, PAGE_READONLY, &old)) return 12;
    /* Adjacent 4 KB page must remain writable. */
    memory[4096] = 44;
    if (memory[0] != 42 || memory[4096] != 44) return 13;
    if (!VirtualFree(memory, 0, MEM_RELEASE)) return 14;
    volatile LONG counter = 0;
    HANDLE threads[4];
    for (unsigned i = 0; i < 4; ++i)
    {
        threads[i] = CreateThread(NULL, 0, worker, (void *)&counter, 0, NULL);
        if (!threads[i]) return 15;
    }
    if (WaitForMultipleObjects(4, threads, TRUE, 10000) != WAIT_OBJECT_0) return 16;
    for (unsigned i = 0; i < 4; ++i) CloseHandle(threads[i]);
    if (counter != 40000) return 17;
    printf("PASS pages=%lu counter=%ld pointer_bits=%u\n", info.dwPageSize, counter,
           (unsigned)(sizeof(void *) * 8));
    return 0;
}
