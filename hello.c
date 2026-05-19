#include<windows.h>

int main(){
    HMODULE hDll = LoadLibraryA("mydll.dll");
    if(hDll== INVALID_HANDLE_VALUE  || hDll==NULL){
        MessageBoxA(NULL,"error in loadig dll","test",MB_OK);
        return EXIT_FAILURE;
    }
    //WHNE 
    BOOL (*func)();
    BOOL (*func2)();
    
    func=(BOOL(*)())GetProcAddress(hDll,"runMe");
    func2 = (BOOL(*)())GetProcAddress(hDll,"runme2");
    if(func==NULL){
        MessageBoxA(NULL,"error in getting function address","test",MB_OK);
        FreeLibrary(hDll);
        return EXIT_FAILURE;
    }
        if(func2==NULL){
        MessageBoxA(NULL,"error in getting function address","test",MB_OK);
        FreeLibrary(hDll);
        return EXIT_FAILURE;
    }
    func();
    func2();
   FreeLibrary(hDll);
   return EXIT_SUCCESS;
}