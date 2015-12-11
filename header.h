#ifndef HEADER_H
#define HEADER_H

typedef struct Header{
  char *servername;
  char *date;
  int contentlength;
  char *contenttype;
  short statuscode;
  char protocol[9]; // HTTP/1.1


}__Header;
typedef __Header* header;

header init_header();






#endif // HEADER_H

