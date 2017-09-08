#ifndef HTTPRESPONSEHEADER_H
#define HTTPRESPONSEHEADER_H

typedef struct HTTPResponseHeader{
  char servername = "mininginx";
  char date[96];
  int contentlength;
  char contenttype = "Content-Type: text/html; charset=utf-8";
  unsigned short statuscode = 200;
  char status = "OK";
  char protocol = "HTTP/1.1"; // must be only HTTP/1.1


}__HTTPResponseHeader;
typedef __HTTPResponseHeader httpResponseHeader;
void HTTPResponseDate(httpResponseHeader*);





#endif // HEADER_H

