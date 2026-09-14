// SPDX-License-Identifier: MIT
// Hidden-window callback/message/timer test; no interactive desktop changes.
#include <windows.h>
#include <stdio.h>
static unsigned messages,timers;
static LRESULT CALLBACK window_proc(HWND window,UINT message,WPARAM wparam,LPARAM lparam) {
 if(message==WM_APP+1){messages++;return (LRESULT)(wparam+17);}
 if(message==WM_TIMER){timers++;return 0;}
 return DefWindowProcA(window,message,wparam,lparam);
}
int main(void) {
 WNDCLASSA cls={0};cls.lpfnWndProc=window_proc;cls.hInstance=GetModuleHandleA(NULL);cls.lpszClassName="AsterRuntimeProbe";
 if(!RegisterClassA(&cls)){printf("RegisterClass error=%lu\n",GetLastError());return 1;}
 HWND window=CreateWindowExA(0,cls.lpszClassName,"Aster hidden runtime probe",WS_OVERLAPPEDWINDOW,0,0,160,100,NULL,NULL,cls.hInstance,NULL);
 if(!window){printf("CreateWindow error=%lu\n",GetLastError());return 2;}
 for(unsigned i=0;i<100;i++)if(SendMessageA(window,WM_APP+1,i,0)!=(LRESULT)(i+17))return 3;
 if(!SetTimer(window,1,10,NULL))return 4;
 DWORD start=GetTickCount();MSG msg;
 while(timers<3 && GetTickCount()-start<5000){
  while(PeekMessageA(&msg,NULL,0,0,PM_REMOVE)){TranslateMessage(&msg);DispatchMessageA(&msg);}
  Sleep(1);
 }
 KillTimer(window,1);DestroyWindow(window);UnregisterClassA(cls.lpszClassName,cls.hInstance);
 printf("messages=%u timers=%u\n",messages,timers);return messages==100 && timers>=3 ? 0 : 5;
}
