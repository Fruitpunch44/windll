#include<windows.h>
#include<stdio.h>
#include<tlhelp32.h>

DWORD get_process(char * process_name){
    HANDLE processsnap;
    HANDLE process;
    PROCESSENTRY32 pe32;
    DWORD priorityclass;

    processsnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(processsnap==INVALID_HANDLE_VALUE){
        fprintf(stdout,"error");
        return EXIT_FAILURE;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    do {
        //fprintf(stdout,"%s\n",pe32.szExeFile);
        priorityclass =0;
        process=OpenProcess(PROCESS_ALL_ACCESS,FALSE,pe32.th32ParentProcessID);
        if(process==INVALID_HANDLE_VALUE){
            return EXIT_FAILURE;
        }
        else{
            priorityclass =GetPriorityClass(process);
            if(!priorityclass){
                CloseHandle(process);
            }
        }
        char *clone=strdup(pe32.szExeFile);
        if(strncmp(process_name,clone,strlen(clone))==0){
            fprintf(stdout,"found process: %s\n",process_name);
            fprintf(stdout,"%d\n",pe32.th32ProcessID);
            return pe32.th32ProcessID;
        }
    }while(Process32Next(processsnap,&pe32));
    fprintf(stdout,"process %s not found\n",process_name);
    return TRUE;


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
        fprintf(stderr,"a error occured");
        return EXIT_FAILURE;
    }
    DWORD PROC_ID = get_process(name);
    fprintf(stdout,"%s",dll_path);

    HANDLE target_process;
    target_process=OpenProcess(PROCESS_ALL_ACCESS,FALSE,PROC_ID);
    if(target_process==INVALID_HANDLE_VALUE){
        fprintf(stderr,"error in opening process");
    }
}