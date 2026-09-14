#include <windows.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    for (int executable = 0; executable < 2; executable++) {
        SetLastError(0);
        HANDLE heap = HeapCreate(executable ? HEAP_CREATE_ENABLE_EXECUTE : 0, 0, 0);
        printf("HeapCreate execute=%d heap=%p error=%lu\n", executable, heap, GetLastError());
        if (!heap) return 2;
        void *p = HeapAlloc(heap, HEAP_ZERO_MEMORY, 64);
        if (!p) return 3;
        *(volatile int *)p = 42;
        if (executable) {
            const unsigned code[] = {0x52800540, 0xd65f03c0};
            memcpy(p, code, sizeof(code));
            FlushInstructionCache(GetCurrentProcess(), p, sizeof(code));
            int result = ((int (*)(void))p)();
            printf("Executable heap code returned %d\n", result);
            if (result != 42) return 4;
        }
        HeapFree(heap, 0, p);
        HeapDestroy(heap);
    }
    return 0;
}
