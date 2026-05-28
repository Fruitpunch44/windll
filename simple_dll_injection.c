#include<windows.h>
#include<stdio.h>
#include<tlhelp32.h>

char *Display_error_message(DWORD error_code){
    LPSTR lpmessagebuff=NULL;

    DWORD dwChars =FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL,error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpmessagebuff,0,NULL);
    
    if(dwChars == 0){
        fprintf(stderr,"error in formating %d",GetLastError());
        return "error";
    }

    return lpmessagebuff;
}

void print_error_message(DWORD error_code){
    char *error_message = Display_error_message(error_code);
    fprintf(stderr," hello from %d :%s\n",error_code,error_message);
    LocalFree(error_message);
}

DWORD get_process(char *process_name){
    HANDLE processsnap;
    HANDLE process;
    PROCESSENTRY32 pe32;
    DWORD priorityclass;

    processsnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(processsnap==INVALID_HANDLE_VALUE){
        DWORD error = GetLastError();
        print_error_message(error);
        return EXIT_FAILURE;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if(!Process32First(processsnap,&pe32)){
        DWORD error =GetLastError();
        char *error_message = Display_error_message(error);
        fprintf(stderr,"%s\n",error_message);
        CloseHandle(processsnap);
        return EXIT_FAILURE;
    }
    do {
        //fprintf(stdout,"%s\n",pe32.szExeFile);
        priorityclass =0;
        process=OpenProcess(PROCESS_ALL_ACCESS,FALSE,pe32.th32ProcessID);
        if(process==NULL){
             continue;
        }
        else{
            priorityclass =GetPriorityClass(process);
            if(!priorityclass){
                CloseHandle(process);
            }
        }
        if(strcmp(process_name,pe32.szExeFile)==0){
            fprintf(stdout,"found process: %s\n",process_name);
            fprintf(stdout,"%d\n",pe32.th32ProcessID);
            CloseHandle(process);
            return pe32.th32ProcessID;
        }
    }while(Process32Next(processsnap,&pe32));
    fprintf(stdout,"process %s not found\n",process_name);
    CloseHandle(processsnap);
}

int main(int argc, char *argv[]){
    if(argc<2){
        fprintf(stdout,"not enough args\n");
    }
    char *name = argv[1];
    char *dll_name = argv[2];
    char dll_path[MAX_PATH]; 

    const DWORD path_res = GetFullPathNameA(dll_name,MAX_PATH,dll_path,NULL);
    if(path_res == 0){
        fprintf(stderr,"a error occured in resloving path\n");
        return EXIT_FAILURE;
    }
    DWORD PROC_ID = get_process(name);
    fprintf(stdout,"path %s\n",dll_path);
    fprintf(stdout,"pid %d\n",PROC_ID);

    HANDLE target_process;
    target_process=OpenProcess(PROCESS_ALL_ACCESS,FALSE,PROC_ID);
    if(target_process==NULL){
        fprintf(stderr,"help from openprocess");
        DWORD error =GetLastError();
        print_error_message(error);
        return EXIT_FAILURE;
    }

    //use virtual alloc
    LPVOID dll_path_address = VirtualAllocEx(target_process,NULL,MAX_PATH,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(dll_path_address==NULL){
        DWORD error =GetLastError();
        fprintf(stderr,"virtual alloc error %d occured\n",error);
        print_error_message(error);
        return EXIT_FAILURE;
    }

    //use write process mem
    DWORD dwwriteres=WriteProcessMemory(target_process,dll_path_address,dll_path,strlen(dll_path)+1,NULL);
      if(dwwriteres==0){
        DWORD error =GetLastError();
        fprintf(stderr," writeprocess memory error %d occured\n",error);
        print_error_message(error);
        return EXIT_FAILURE;
    }


    //call getmodule
    HMODULE module =GetModuleHandleA("kernel32.dll");
    if(module ==NULL){
        DWORD error =GetLastError();
        fprintf(stderr,"get module error %d occured\n",error);
        print_error_message(error);
        return EXIT_FAILURE;
    }

    //GET PROCEDURE
    FARPROC func_address=GetProcAddress(module,"LoadLibraryA");
      if(func_address==NULL){
        DWORD error =GetLastError();
        fprintf(stderr,"get procaddress error %d occured\n",error);
        print_error_message(error);
        return EXIT_FAILURE;
    }

    HANDLE htread_creation =CreateRemoteThread(target_process,NULL,0,(LPTHREAD_START_ROUTINE)func_address,dll_path_address,0,NULL);
    if(htread_creation==NULL){
        DWORD error =GetLastError();
        fprintf(stderr,"create remote thread error %d occured\n",error);
        print_error_message(error);
        return EXIT_FAILURE;
    }
        
    fprintf(stdout,"dll injection successful\n");
    return 0;
}