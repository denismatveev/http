#ifndef HTTPREQUESTHEADER
#define HTTPREQUESTHEADER

typedef struct HTTPRequestHeader {
  char proto[12];
  char method[16];// GET or HEAD
  char *params;// string after question sign(?)
  char *path;
} __HTTPRequestHeader;

typedef __HTTPRequestHeader* httpRequestHeader;

httpRequestHeader httpRequestHeaderInit();
void httpRequestHeaderRemove();

#endif // REQUEST

