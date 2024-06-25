#define BUFFER_SIZE 1024

u8 isHeader(char *restrict str1, char *restrict str2){
    u32 len = strlen(str1);
    for(u32 x=0; x<len; x++){
        if(str1[x] != str2[x]) return FALSE;
    };
    return TRUE;
};
void handleClient(void *arg){
    s64 clientFd = (s64)arg;
    char *buff = (char*)malloc(BUFFER_SIZE);
    u64 bytesRec = recv(clientFd, buff, BUFFER_SIZE, 0);
    if(bytesRec > 0){
        if(isHeader("GET", buff)){
            //TODO: parse get
        };
    };
    free(buff);
};