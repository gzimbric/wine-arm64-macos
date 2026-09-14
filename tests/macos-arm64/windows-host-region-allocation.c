// SPDX-License-Identifier: MIT
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
typedef void* (WINAPI *Alloc2)(HANDLE,void*,SIZE_T,ULONG,ULONG,MEM_EXTENDED_PARAMETER*,ULONG);
int main(void){
 Alloc2 alloc=(Alloc2)GetProcAddress(GetModuleHandleA("kernelbase.dll"),"VirtualAlloc2");if(!alloc)return 2;
 MEM_ADDRESS_REQUIREMENTS req={(void*)0x7100000000ULL,(void*)0x713fffffffULL,0x10000};
 MEM_EXTENDED_PARAMETER par={0};par.Type=MemExtendedParameterAddressRequirements;par.Pointer=&req;
 LARGE_INTEGER a,b,f;QueryPerformanceFrequency(&f);QueryPerformanceCounter(&a);
 for(int direction=0;direction<2;direction++)for(int i=0;i<50;i++){
  void*p=alloc(GetCurrentProcess(),0,65536,MEM_RESERVE|MEM_COMMIT|(direction?MEM_TOP_DOWN:0),PAGE_READWRITE,&par,1);
  if(!p){printf("allocation failed %lu\n",GetLastError());return 3;}
  if((uintptr_t)p<0x7110000000ULL || (uintptr_t)p+65536>0x7138000000ULL){printf("overlap %p\n",p);return 4;}
  *(volatile int*)p=42;if(*(volatile int*)p!=42)return 5;VirtualFree(p,0,MEM_RELEASE);
 }
 QueryPerformanceCounter(&b);printf("PASS 100 constrained allocations, both directions, %.3f ms\n",1000.0*(b.QuadPart-a.QuadPart)/f.QuadPart);return 0;
}
