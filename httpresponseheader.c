#include"httpresponseheader.h"
#include<time.h>
void HTTPResponseDate(httpResponseHeader* header)
{
    time_t result = time(NULL);
    char date[96] =  asctime (result);
    strncmp(date, header->date, 96);
}
