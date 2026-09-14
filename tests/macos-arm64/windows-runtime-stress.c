// SPDX-License-Identifier: MIT
// Bounded correctness/throughput workload for experimental Wine runtimes.
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define THREADS 8
#define ITERATIONS 20000000
static uint64_t results[THREADS];
static DWORD WINAPI worker(void *arg) {
 uintptr_t id=(uintptr_t)arg;uint64_t x=id+1;
 for(unsigned i=0;i<ITERATIONS;i++){x^=x<<13;x^=x>>7;x^=x<<17;}
 results[id]=x;return 0;
}
int main(void) {
 HANDLE threads[THREADS];LARGE_INTEGER f,a,b;uint64_t expected[THREADS];
 if(!QueryPerformanceFrequency(&f)||!QueryPerformanceCounter(&a))return 1;
 for(uintptr_t i=0;i<THREADS;i++){threads[i]=CreateThread(0,0,worker,(void*)i,0,0);if(!threads[i])return 2;}
 if(WaitForMultipleObjects(THREADS,threads,TRUE,60000)!=WAIT_OBJECT_0)return 3;
 if(!QueryPerformanceCounter(&b))return 4;
 for(uintptr_t i=0;i<THREADS;i++){expected[i]=results[i];CloseHandle(threads[i]);}
 double elapsed=1000.0*(b.QuadPart-a.QuadPart)/f.QuadPart;
 // Independently repeat serially; check both concurrency and deterministic output.
 for(uintptr_t i=0;i<THREADS;i++){worker((void*)i);if(results[i]!=expected[i])return 5;}
#if defined(__x86_64__)
 unsigned char *code=VirtualAlloc(0,4096,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
 if(!code)return 6;
 code[0]=0xb8;code[1]=42;code[2]=code[3]=code[4]=0;code[5]=0xc3;
 FlushInstructionCache(GetCurrentProcess(),code,6);
 int (*fn)(void)=(int(*)(void))code;
 if(fn()!=42)return 7;
 code[1]=43;FlushInstructionCache(GetCurrentProcess(),code,6);
 if(fn()!=43)return 8;
 VirtualFree(code,0,MEM_RELEASE);
#endif
 printf("PASS threads=%d iterations=%d parallel_ms=%.3f checksum=%llx\n",THREADS,ITERATIONS,elapsed,(unsigned long long)results[0]);
 return 0;
}
