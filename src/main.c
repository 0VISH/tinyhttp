#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "basic.h"

#define tlog(...) fprintf(config.log, __VA_ARGS__)

typedef struct{
    char *addr;
    void *log;
    u32   port;
    u32   threads;
} Config;

Config config;
u8 running;

#include "client.c"

int main(int argc, char **argv){
    config.port = 8080;
    config.threads = 1;
    config.log = stdout;
    config.addr = "127.0.0.1";  //localhost
    for(u32 x=1; x<argc; x++){
        char *arg = argv[x];
        if(arg[2] == ':'){
            switch(arg[1]){
                case 'p': config.port = (u32)atoi(&arg[3]);break;
                case 't': config.threads = (u32)atoi(&arg[3]);break;
                case 'l': config.log = &arg[3];break;
                default: printf("ignoring unkown switch '%c'\n", arg[1]);break;
            };
        }else{
            printf("incorrect switch \"%s\"\n", arg);
            return EXIT_SUCCESS;
        };
    };
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0){
        tlog("[-] startup failed\n");
        return EXIT_FAILURE;
    };
    SYSTEMTIME t;
    GetLocalTime(&t);
    tlog("[+] server online @ %d/%d/%d(%d:%d)\n", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute);

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //IPv4, reliable 2-way connection socket, TCP protocol
    if(s == INVALID_SOCKET){
        tlog("[-] could not create socket @ port:%d\n", config.port);
        WSACleanup();
        goto EXIT;
    };
    struct sockaddr_in sockinfo;
    sockinfo.sin_family = AF_INET;
    sockinfo.sin_port = htons(config.port);
    sockinfo.sin_addr.s_addr = inet_addr(config.addr);
    if(bind(s, (const struct sockaddr*)&sockinfo, sizeof(sockinfo)) == SOCKET_ERROR){
        tlog("[-] binding failed[%s:%d]\n", config.addr, config.port);
        goto CLEANUP;
    };
    tlog("[+] successful binding[%s:%d]\n", config.addr, config.port);
    if(listen(s, SOMAXCONN) == SOCKET_ERROR){
        tlog("[-] could not listen on socket\n");
        goto CLEANUP;
    };
    tlog("[+] listening...\n");
    running = TRUE;
    while(running){
        struct sockaddr_in clientinfo;
        u32 clientinfoSize = sizeof(clientinfo);
        s64 clientFd = accept(s, (struct sockaddr*)&clientinfo, &clientinfoSize);
        if(clientFd == SOCKET_ERROR){
            tlog("[-] could not accept connection\n");
            continue;
        };
        tlog("[+] accepted connection\n");
        handleClient((void*)clientFd);
    };

CLEANUP:
    closesocket(s);
    WSACleanup();
EXIT:
    GetLocalTime(&t);
    tlog("[+] server offline @ %d/%d/%d(%d:%d)\n", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute);
    return EXIT_SUCCESS;
};

#pragma comment(lib, "ws2_32.lib")