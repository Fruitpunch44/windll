#include<windows.h>

#pragma comment(lib, "user32.lib")



BOOL APIENTRY Dllmain(HMODULE hmodule,DWORD reason_for_call,LPVOID lpReserved){
    switch(reason_for_call){
        case DLL_PROCESS_ATTACH:
                MessageBoxA(NULL,"DLL Loaded!","DLL Message",MB_OK);
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
__declspec(dllexport) BOOL WINAPI runMe(void){
    MessageBoxA(NULL,"Hello from the DLL!","DLL Message",MB_OK);
    return TRUE;
}
__declspec(dllexport) BOOL WINAPI runme2(void){
    MessageBoxA(NULL,"Hello from the DLL2! running notepad","DLL Message",MB_OK);
    system("start notepad.exe");
    return TRUE;
}