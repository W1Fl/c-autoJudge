//
// Created by pipiFan on 2019/2/27.
//

#ifndef UNTITLED1_PROCESS_H
#define UNTITLED1_PROCESS_H

#endif //UNTITLED1_PROCESS_H

#include <windows.h>
#include <cstdio>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define OutBUFSIZE 1024
#define BUFSIZE 1024
#define dwSize 1024
#define TimeLimit 2000
SECURITY_ATTRIBUTES sa;

PCVOID in_buffer;


STARTUPINFO st ;
PROCESS_INFORMATION pi;
HANDLE hOutPut,hInPut,pOutPut,pInPut;
char buffer[OutBUFSIZE];

BOOL _WriteToPipe()
{
    ///主进程写入管道
    DWORD dwWritten;
    BOOL bSuccess = FALSE;

    bSuccess = WriteFile( pInPut, in_buffer, dwSize, &dwWritten, NULL);
    return bSuccess;
}

int run(char *cmd)
{
    ///执行进程
    DWORD readByte = 0;
    int len = 0;
    BOOL hResult ;


    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle =TRUE;
    sa.lpSecurityDescriptor = NULL;

    if  (!CreatePipe(&pOutPut, &pInPut,&sa, 0))
        return -1;

    if(!CreatePipe(&hOutPut,&hInPut,&sa,0))
    {
        printf("create pipe failed");
        return 1;
    }

    st.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&st);
    st.hStdOutput = hInPut;
    st.hStdError = hInPut;
    st.wShowWindow = SW_HIDE;
    st.hStdInput  = pOutPut;
    st.dwFlags = STARTF_USESHOWWINDOW |STARTF_USESTDHANDLES;
    if(!CreateProcess(NULL,cmd,NULL,
                      NULL,TRUE,NULL,NULL,NULL,&st,&pi))
    {
        return 1;
    }
    _WriteToPipe();
    DWORD dwRet = WaitForSingleObject(pi.hProcess,TimeLimit);
    switch(dwRet)
    {
        case WAIT_TIMEOUT:
        case WAIT_FAILED: {
            CloseHandle(hInPut);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            CloseHandle(hOutPut);
//            TerminateProcess(pi.hProcess,0);
            popen("taskkill -F -im a.exe","r");
            return 1;
        }
        case WAIT_OBJECT_0:
            CloseHandle(hInPut);//close hInPut handle ,make hte write pipe completes
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

    }

    memset(buffer,0,sizeof(buffer));
    do
    {
        hResult = ReadFile(hOutPut,buffer+len,BUFSIZE,&readByte,NULL);
        len += readByte;
        Sleep(200);
    }
    while(readByte!=0 && hResult);

    printf("%s\n",buffer);

    CloseHandle(hOutPut);
    return 0;
}