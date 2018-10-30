#include "http.h"
#include "string.h"


int main (int argc, char** argv)
{


http_reason_code_t rt=200;

char str[128];
http_protocol_version_t p=0;


create_status_line(str,128,p,rt);

puts(str);
}
