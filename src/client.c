#define BUFFER_SIZE 1024
#define FILE_NAME_SIZE 50

char *fourOfour = "404 error. Page not found";
typedef struct{
    char *ext;
    char *mime;
}MimeEntry;
MimeEntry mimeTable[] = {
    {"jpg",  "image/jpeg"},
    {"png",  "imagem/png"},
    {"pdf",  "application/pdf"},
    {"html", "text/html"},
    {"css",  "text/css"},
    {"js",   "text/javascript"},
};

u8 isHeader(char *restrict str1, char *restrict str2){
    u32 len = strlen(str1);
    for(u32 x=0; x<len; x++){
        if(str1[x] != str2[x]) return FALSE;
    };
    return TRUE;
};
void buildHttpResponse(char *buff, char *fileName){
    char *fileExt = fileName;
    while(*fileExt != '.'){
        fileExt++;
        if(*fileExt == '\0'){goto FILE_NOT_FOUND;};
    };
    fileExt++;
    char *mime = NULL;
    for(u32 x=0; x<ARRAY_LENGTH(mimeTable); x++){
        MimeEntry *me = &mimeTable[x];
        if(strcmp(me->ext, fileExt) == 0){
            mime = me->mime;
            break;
        };
    };
    if(mime == NULL){
        snprintf(buff, BUFFER_SIZE,
                 "HTTP/1.1 501 Not Implemented\n"
                 "Content-Type: text/plain\n"
                 "\n"
                 "Mime not implemented for file extension %s\n",
                 fileExt);
        tlog("[-] Mime not implemented for file extension %s\n", fileExt);
        return;
    };
    FILE *f = fopen(fileName, "r");
    if(f == NULL){
FILE_NOT_FOUND:
        snprintf(buff, BUFFER_SIZE,
                 "HTTP/1.1 404 Not Found\n"
                 "Content-Type: text/plain\n"
                 "\n"
                 "file %s not found\n",
                 fileName);
        tlog("[-] file %s not found\n", fileName);
        return;
    };
    snprintf(buff, BUFFER_SIZE,
             "HTTP/1.1 200 OK\n"
             "Content-Type: %s\n"
             "\n",
             mime);
    const u32 buffEaten = strlen(buff);
    const u32 buffLeftoverSize = BUFFER_SIZE - buffEaten;
    fread(buff+buffEaten, sizeof(char), buffLeftoverSize, f);
    fclose(f);
};
void handleClient(void *arg){
    s64 clientFd = (s64)arg;
    char clientMsgMem[BUFFER_SIZE];
    char *buff = clientMsgMem;
    u64 bytesRec = recv(clientFd, buff, BUFFER_SIZE, 0);
    if(bytesRec > 0){
        if(isHeader("GET", buff)){
            buff = buff + 4;
            u32 x = 0;
            char fileName[FILE_NAME_SIZE];
            while(buff[x] != ' ' && buff[x] != '\n') x++;
            memcpy(fileName, buff, x);
            fileName[x] = '\0';
            if(x == 1 && fileName[0] == '/') strcpy(fileName, "/index.html");
            tlog("[+] serving %s\n", fileName);
            char serverMsgMem[BUFFER_SIZE];
            buildHttpResponse(serverMsgMem, fileName+1);
            send(clientFd, serverMsgMem, strlen(serverMsgMem), 0);
        };
    };
};
