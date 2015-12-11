#ifndef HTTPRESPONSEHEADER_H
#define HTTPRESPONSEHEADER_H

typedef struct HTTPResponseHeader{
  char *servername;
  char *date;
  int contentlength;
  char *contenttype;
  unsigned short statuscode;
  char protocol[9]; // must be only HTTP/1.1


}__HTTPResponseHeader;
typedef __HTTPResponseHeader* httpResponseHeader;

httpResponseHeader init_httpResponseHeader(char*,char*,int, unsigned short, char*);
void httpResponseHeaderRemove();






#endif // HEADER_H

