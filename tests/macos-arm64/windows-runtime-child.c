// SPDX-License-Identifier: MIT
#include <windows.h>
#include <stdio.h>
int main(int argc,char **argv) {
 (void)argv;
 if(argc>1){SYSTEM_INFO info;GetSystemInfo(&info);return info.dwPageSize==4096 ? 42 : 2;}
 char path[MAX_PATH],command[MAX_PATH+32];
 if(!GetModuleFileNameA(NULL,path,sizeof(path)))return 1;
 if(snprintf(command,sizeof(command),"\"%s\" --child",path)<0)return 2;
 STARTUPINFOA start={0};PROCESS_INFORMATION process={0};start.cb=sizeof(start);
 if(!CreateProcessA(NULL,command,NULL,NULL,FALSE,0,NULL,NULL,&start,&process)){printf("CreateProcess failed %lu\n",GetLastError());return 3;}
 if(WaitForSingleObject(process.hProcess,15000)!=WAIT_OBJECT_0){TerminateProcess(process.hProcess,9);return 4;}
 DWORD result=0;if(!GetExitCodeProcess(process.hProcess,&result))return 5;
 CloseHandle(process.hThread);CloseHandle(process.hProcess);
 printf("child_exit=%lu\n",result);return result==42 ? 0 : 6;
}
