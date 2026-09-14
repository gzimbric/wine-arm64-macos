// Diagnostic only: query kernel support and test a 4 KB child process launch.
#include <os/arch/arm64.h>
#include <spawn.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <sys/mman.h>
#include <mach/mach.h>
#include <pthread.h>
#include <signal.h>
extern char **environ;
extern kern_return_t thread_set_x86_64_compat(uint32_t enable);

static void *test_thread(void *unused) {
 (void)unused;
 kern_return_t enabled=thread_set_x86_64_compat(1);
 kern_return_t restored=enabled==KERN_SUCCESS ? thread_set_x86_64_compat(0) : KERN_FAILURE;
 printf("tso_enable=%d tso_restore=%d\n",enabled,restored);
 // No macOS calls while custom ABI mode is active, except its documented APIs.
 sigset_t all,old;sigfillset(&all);
 int blocked=pthread_sigmask(SIG_BLOCK,&all,&old);
 if(blocked)return (void *)(uintptr_t)1;
 int initial=os_custom_x18_abi_enabled();
 if(initial)return (void *)(uintptr_t)1;
 os_set_custom_x18_abi_enabled(true);
 int active=os_custom_x18_abi_enabled();
 uint64_t written=0x123456789abcdef0ULL,readback;
 __asm__ volatile("mov x18, %1\n\tmov %0, x18" : "=r"(readback) : "r"(written) : "x18");
 os_set_custom_x18_abi_enabled(false);
 int final=os_custom_x18_abi_enabled();
 int unblocked=pthread_sigmask(SIG_SETMASK,&old,NULL);
 printf("x18_initial=%d x18_active=%d x18_final=%d x18_roundtrip=%d\n",initial,active,final,readback==written);
 return (void *)(uintptr_t)(enabled!=KERN_SUCCESS || restored!=KERN_SUCCESS || !active || final || readback!=written || unblocked);
}

static int child_tests(void) {
 int failed=getpagesize()!=4096;
 printf("child_page_size=%d\n",getpagesize());
 uintptr_t addresses[]={0x10000000,0x7ffe0000,0x80000000};
 for(unsigned i=0;i<sizeof addresses/sizeof addresses[0];i++) {
  errno=0;
  void *p=mmap((void *)addresses[i],4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON|MAP_FIXED,-1,0);
  int error=errno,valid=0;
  if(p!=MAP_FAILED){*(volatile unsigned char *)p=42;valid=*(volatile unsigned char *)p==42;munmap(p,4096);}
  printf("low_va=%lx errno=%d readback_ok=%d\n",(unsigned long)addresses[i],error,valid);
  failed|=!valid;
 }
 pthread_t thread;void *result;
 if(pthread_create(&thread,NULL,test_thread,NULL))return 1;
 if(pthread_join(thread,&result))return 1;
 return failed || (uintptr_t)result;
}
int main(int argc,char **argv) {
 if(argc>1 && !strcmp(argv[1],"--child")) {
  return child_tests();
 }
 printf("kernel_cross_arch_support=%d parent_page_size=%d\n",os_cross_arch_is_supported(OS_CROSS_ARCH_X86_64),getpagesize());
 posix_spawnattr_t attr;int error=posix_spawnattr_init(&attr);if(error)return error;
 error=posix_spawnattr_set_4k_page_size_np(&attr);printf("set_4k_attribute=%d (%s)\n",error,strerror(error));
 if(!error) {
  pid_t child;char *args[]={argv[0],"--child",NULL};
  error=posix_spawn(&child,argv[0],NULL,&attr,args,environ);printf("spawn_4k_child=%d (%s)\n",error,strerror(error));
  if(!error){
   int status;
   while(waitpid(child,&status,0)<0) { if(errno!=EINTR) return 3; }
   printf("child_status=%d\n",status);
   posix_spawnattr_destroy(&attr);
   return WIFEXITED(status) ? WEXITSTATUS(status) : 3;
  }
 }
 posix_spawnattr_destroy(&attr);return error ? 1 : 0;
}
