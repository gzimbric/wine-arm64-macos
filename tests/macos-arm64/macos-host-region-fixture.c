// SPDX-License-Identifier: MIT
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
/* Inject only into the isolated Wine allocation test. Native reservations are
 * deliberately outside Wine's own view tree. The Windows companion checks that
 * constrained allocations in both directions avoid them. */
__attribute__((constructor)) static void reserve_host_regions(void) {
 uintptr_t bases[]={0x7100000000ULL,0x7138000000ULL};size_t sizes[]={0x10000000,0x08000000};
 for(int i=0;i<2;i++){
  void *p=mmap((void*)bases[i],sizes[i],PROT_NONE,MAP_PRIVATE|MAP_ANON,-1,0);
  if(p!=(void*)bases[i]){if(p!=MAP_FAILED)munmap(p,sizes[i]);fprintf(stderr,"HOST REGION FAIL %d\n",i);_exit(70);}
  else fprintf(stderr,"HOST REGION READY %d\n",i);
 }
}
