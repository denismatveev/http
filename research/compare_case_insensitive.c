#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define TOLOWERCASE(c)      (char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define TOUPPERCASE(c)      (char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)
typedef char u_int_t;
typedef enum __request_header
{
    Accept = 0,
    Accept_Charset,
    Accept_Encoding,
    Accept_Language,
    Authorization,
    Expect,
    From,
    Host,
    If_Match,
    If_Modified_Since,
    If_None_Match,
    If_Range,
    If_Unmodified_Since,
    Max_Forwards,
    Proxy_Authorization,
    Range,
    Referer,
    TE,
    User_Agent,
    INVALID_REQUEST_HEADER=-1

}http_request_header_t;
static char* request_header[] =
{
    "Accept:",
    "Accept-Charset:",
    "Accept-Encoding:",
    "Accept-Language:",
    "Authorization:",
    "Expect:",
    "From:",
    "Host:",
    "If-Match:",
    "If-Modified-Since:",
    "If-None-Match:",
    "If-Range:",
    "If-Unmodified-Since:",
    "Max-Forwards:",
    "Proxy-Authorization:",
    "Range:",
    "Referer:",
    "TE:",
    "User-Agent:",
    NULL
};
http_request_header_t find_http_request_header_switch_for(const char *h)
{
    http_request_header_t result = Accept; /* value corresponding to etable[0] */
    char ch = h[0];
    u_int_t i=0; // initial position for searching
    switch (ch)
    {
    case 'A':
    case 'a':
        i=0;
        break;
    case 'I':
    case 'i':
        i=8;
        result=If_Match;
        break;
    case 'H':
    case 'h':
        result=Host;
        i=7;
        break;
    case 'U':
    case 'u':
        result=User_Agent;
        i=18;
        break;
    default:
        break;
    }
    for (;request_header[i] != NULL; ++i, ++result)
        if (!(strncasecmp(h, request_header[i], 16)))
            return result;
    return INVALID_REQUEST_HEADER;
}
http_request_header_t find_http_request_header_switch(const char *h)
{

    switch(h[0])
    {
    //Authorization:
    case 'A':
    case 'a':
        switch(h[1])
        {
        case 'U':
        case 'u':
            switch(h[2])
            {
            case 'T':
            case 't':
                switch(h[3])
                {
                case 'H':
                case 'h':
                    switch (h[4])
                    {
                    case 'O':
                    case 'o':
                        switch (h[5])
                        {
                        case 'R':
                        case 'r':
                            switch(h[6])
                            {
                            case 'I':
                            case 'i':
                                switch (h[7])
                                {
                                case 'Z':
                                case 'z':
                                    switch(h[8])
                                    {
                                    case 'A':
                                    case 'a':
                                        switch (h[9])
                                        {
                                        case 'T':
                                        case 't':
                                            switch (h[10])
                                            {
                                            case 'I':
                                            case 'i':
                                                switch (h[11])
                                                {
                                                case 'O':
                                                case 'o':
                                                    switch (h[12])
                                                    {
                                                    case 'N':
                                                    case 'n':
                                                        switch (h[13])
                                                        {
                                                        case ':':
                                                            return Authorization;
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            }
                            break;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;

        case 'C':
        case 'c':
            switch(h[2])
            {
            case 'C':
            case 'c':
                switch(h[3])
                {
                case 'E':
                case 'e':
                    switch (h[4])
                    {
                    case 'P':
                    case 'p':
                        switch (h[5])
                        {
                        case 'T':
                        case 't':
                            switch (h[6])
                            {
                            case ':':
                                return Accept;
                            case '-':
                                switch(h[7])
                                {//Accept-Charset
                                case 'C':
                                case 'c':
                                    switch(h[8])
                                    {
                                    case 'H':
                                    case 'h':
                                        switch (h[9])
                                        {
                                        case 'A':
                                        case 'a':
                                            switch (h[10])
                                            {
                                            case 'R':
                                            case 'r':
                                                switch (h[11])
                                                {
                                                case 'S':
                                                case 's':
                                                    switch (h[12])
                                                    {
                                                    case 'E':
                                                    case 'e':
                                                        switch (h[13])
                                                        {
                                                        case 'T':
                                                        case 't':
                                                            switch (h[14])
                                                            {
                                                            case ':':
                                                                return Accept_Charset;
                                                            }
                                                            break;
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                    //Accept-Encoding
                                case 'E':
                                case 'e':
                                    switch(h[8])
                                    {
                                    case 'N':
                                    case 'n':
                                        switch (h[9])
                                        {
                                        case 'C':
                                        case 'c':
                                            switch (h[10])
                                            {
                                            case 'O':
                                            case 'o':
                                                switch (h[11])
                                                {
                                                case 'D':
                                                case 'd':
                                                    switch (h[12])
                                                    {
                                                    case 'I':
                                                    case 'i':
                                                        switch (h[13])
                                                        {
                                                        case 'N':
                                                        case 'n':
                                                            switch (h[14])
                                                            {
                                                            case 'G':
                                                            case 'g':
                                                                switch (h[15])
                                                                {
                                                                case ':':
                                                                    return Accept_Encoding;

                                                                }
                                                                break;

                                                            }
                                                            break;

                                                        }
                                                        break;

                                                    }
                                                    break;

                                                }
                                                break;

                                            }
                                            break;

                                        }
                                        break;
                                    }
                                    break;
                                    //Accept-Language
                                case 'L':
                                case 'l':
                                    switch (h[8])
                                    {
                                    case 'A':
                                    case 'a':
                                        switch (h[9])
                                        {
                                        case 'N':
                                        case 'n':
                                            switch (h[10])
                                            {
                                            case 'G':
                                            case 'g':
                                                switch (h[11])
                                                {
                                                case 'U':
                                                case 'u':
                                                    switch (h[12])
                                                    {
                                                    case 'A':
                                                    case 'a':
                                                        switch (h[13])
                                                        {
                                                        case 'G':
                                                        case 'g':
                                                            switch (h[14])
                                                            {
                                                            case 'E':
                                                            case 'e':
                                                                switch (h[15])
                                                                {
                                                                case ':':
                                                                    return Accept_Language;

                                                                }
                                                                break;

                                                            }
                                                            break;

                                                        }
                                                        break;

                                                    }
                                                    break;

                                                }
                                                break;

                                            }
                                            break;

                                        }
                                        break;
                                    }

                                    break;
                                }
                                break;
                            }
                            break;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;
    case 'E':
    case 'e':
        switch (h[1])
        {
        case 'X':
        case 'x':
            switch(h[2])
            {
            case 'P':
            case 'p':
                switch (h[3])
                {
                case 'E':
                case 'e':
                    switch (h[4])
                    {
                    case 'C':
                    case 'c':
                        switch (h[5])
                        {
                        case 'T':
                        case 't':
                            switch (h[6])
                            {
                            case ':':
                                return Expect;

                            }
                            break;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;
    case 'F':
    case 'f':
        switch (h[1])
        {
        case 'R':
        case 'r':
            switch (h[2])
            {
            case 'O':
            case 'o':
                switch(h[3])
                {
                case 'M':
                case 'm':
                    switch (h[4])
                    {
                    case ':':
                        return From;

                    }
                    break;

                }
                break;

            }
            break;
        }
        break;
    case 'H':
    case 'h':
        switch (h[1])
        {
        case 'O':
        case 'o':
            switch (h[2])
            {
            case 'S':
            case 's':
                switch (h[3])
                {
                case 'T':
                case 't':
                    switch (h[4])
                    {
                    case ':':
                        return Host;

                    }
                    break;
                }
                break;
            }
            break;
        }
        break;

    default:
        return INVALID_REQUEST_HEADER;

    }

    return INVALID_REQUEST_HEADER;

}


http_request_header_t find_http_request_header_if_else(const char *reqh)
{

    // Accept
    if(TOLOWERCASE(reqh[0]) == 'a' && TOLOWERCASE(reqh[1]) == 'c' && TOLOWERCASE(reqh[2]) == 'c' && TOLOWERCASE(reqh[3]) == 'e' && TOLOWERCASE(reqh[4]) == 'p' && TOLOWERCASE(reqh[5]) == 't' && reqh[6] == ':')
        return Accept;
    //Accept-Charset
    else if(TOLOWERCASE(reqh[0]) == 'a' && TOLOWERCASE(reqh[1]) == 'c' && TOLOWERCASE(reqh[2]) == 'c' && TOLOWERCASE(reqh[3]) == 'e' && TOLOWERCASE(reqh[4]) == 'p' && TOLOWERCASE(reqh[5]) == 't' && reqh[6] == '-'\
            && TOLOWERCASE(reqh[7]) == 'c' && TOLOWERCASE(reqh[8]) == 'h' && TOLOWERCASE(reqh[9]) == 'a' && TOLOWERCASE(reqh[10]) == 'r' && TOLOWERCASE(reqh[11]) == 's' && TOLOWERCASE(reqh[12]) == 'e' && TOLOWERCASE(reqh[13]) == 't' && reqh[14] == ':')
        return Accept_Charset;
    //Accept-Encoding
    else if(TOLOWERCASE(reqh[0]) == 'a' && TOLOWERCASE(reqh[1]) == 'c' && TOLOWERCASE(reqh[2]) == 'c' && TOLOWERCASE(reqh[3]) == 'e' && TOLOWERCASE(reqh[4]) == 'p' && TOLOWERCASE(reqh[5]) == 't' && reqh[6] == '-'\
            && TOLOWERCASE(reqh[7]) == 'e' && TOLOWERCASE(reqh[8]) == 'n' && TOLOWERCASE(reqh[9]) == 'c' && TOLOWERCASE(reqh[10]) == 'o' && TOLOWERCASE(reqh[11]) == 'd'\
            && TOLOWERCASE(reqh[12]) == 'i' && TOLOWERCASE(reqh[13]) == 'n' && TOLOWERCASE(reqh[13]) == 'g' && reqh[14] == ':')
        return Accept_Encoding;
    //Accept-Language
    else if(TOLOWERCASE(reqh[0]) == 'a' && TOLOWERCASE(reqh[1]) == 'c' && TOLOWERCASE(reqh[2]) == 'c' && TOLOWERCASE(reqh[3]) == 'e' && TOLOWERCASE(reqh[4]) == 'p' && TOLOWERCASE(reqh[5]) == 't' && reqh[6] == '-'\
            && TOLOWERCASE(reqh[7]) == 'l' && TOLOWERCASE(reqh[8]) == 'a' && TOLOWERCASE(reqh[9]) == 'n' && TOLOWERCASE(reqh[10]) == 'g' && TOLOWERCASE(reqh[11]) == 'u'\
            && TOLOWERCASE(reqh[12]) == 'a' && TOLOWERCASE(reqh[13]) == 'g' && TOLOWERCASE(reqh[14]) == 'e' && reqh[15] == ':')
        return Accept_Language;
    // Authorization
    else if (TOLOWERCASE(reqh[0]) == 'a' && TOLOWERCASE(reqh[1]) == 'u' && TOLOWERCASE(reqh[2]) == 't' && TOLOWERCASE(reqh[3]) == 'h' && TOLOWERCASE(reqh[4]) == 'o' && TOLOWERCASE(reqh[5]) == 'r' && TOLOWERCASE(reqh[6]) == 'i'\
             && TOLOWERCASE(reqh[7]) == 'z' && TOLOWERCASE(reqh[8]) == 'a' && TOLOWERCASE(reqh[9]) == 't' && TOLOWERCASE(reqh[10]) == 'i' && TOLOWERCASE(reqh[11]) == 'o' && TOLOWERCASE(reqh[12]) == 'n' && reqh[13] == ':')
        return Authorization;
    //Expect
    else if (TOLOWERCASE(reqh[0]) == 'e' && TOLOWERCASE(reqh[1]) == 'x' && TOLOWERCASE(reqh[2]) == 'p' && TOLOWERCASE(reqh[3]) == 'e' && TOLOWERCASE(reqh[4]) == 'c' && TOLOWERCASE(reqh[5]) == 't' && reqh[6] == ':')
        return Expect;
    //From
    else if (TOLOWERCASE(reqh[0]) == 'f' && TOLOWERCASE(reqh[1]) == 'r' && TOLOWERCASE(reqh[2]) == 'o' && TOLOWERCASE(reqh[3]) == 'm' && reqh[4] == ':')
        return From;
    //Host
    else if (TOLOWERCASE(reqh[0]) == 'h' && TOLOWERCASE(reqh[1]) == 'o' && TOLOWERCASE(reqh[2]) == 's' && TOLOWERCASE(reqh[3]) == 't' && reqh[4] == ':')
        return Host;
    //If-Match
    else if (TOLOWERCASE(reqh[0]) == 'i' && TOLOWERCASE(reqh[1]) == 'f' && reqh[2] == '-' && TOLOWERCASE(reqh[3]) == 'm' && TOLOWERCASE(reqh[4]) == 'a' && TOLOWERCASE(reqh[5]) == 't' && TOLOWERCASE(reqh[6]) == 'c'\
             && TOLOWERCASE(reqh[7]) == 'h' && reqh[8] == ':')
        return If_Match;
    //If-Modified-Since
    else if(TOLOWERCASE(reqh[0]) == 'i' && TOLOWERCASE(reqh[1]) == 'f' && reqh[2] == '-' && TOLOWERCASE(reqh[3]) == 'm' && TOLOWERCASE(reqh[4]) == 'o' && TOLOWERCASE(reqh[5]) == 'd' && TOLOWERCASE(reqh[6]) == 'i'\
            && TOLOWERCASE(reqh[7]) == 'f' && TOLOWERCASE(reqh[8]) == 'i' && TOLOWERCASE(reqh[9]) == 'e' && TOLOWERCASE(reqh[10]) == 'd' && reqh[11] == '-' && TOLOWERCASE(reqh[12]) == 's' && TOLOWERCASE(reqh[13]) == 'i' && TOLOWERCASE(reqh[14]) == 'n'\
            && TOLOWERCASE(reqh[15]) == 'c' && TOLOWERCASE(reqh[16]) == 'e' && reqh[17] == ':')
        return If_Modified_Since;
    //If-None-Match
    else if(TOLOWERCASE(reqh[0]) == 'i' && TOLOWERCASE(reqh[1]) == 'f' && reqh[2] == '-' && TOLOWERCASE(reqh[3]) == 'n' && TOLOWERCASE(reqh[4]) == 'o' && TOLOWERCASE(reqh[5]) == 'n' && TOLOWERCASE(reqh[6]) == 'e' && reqh[7] == '-'\
            && TOLOWERCASE(reqh[8]) == 'm' && TOLOWERCASE(reqh[9]) == 'a' && TOLOWERCASE(reqh[10]) == 't' && TOLOWERCASE(reqh[11]) == 'c' && TOLOWERCASE(reqh[12]) == 'h' && reqh[13] == ':')
        return If_None_Match;
    //If-Range
    else if(TOLOWERCASE(reqh[0]) == 'i' && TOLOWERCASE(reqh[1]) == 'f' && reqh[2] == '-' && TOLOWERCASE(reqh[3]) == 'r' && TOLOWERCASE(reqh[4]) == 'a' && TOLOWERCASE(reqh[5]) == 'n' && TOLOWERCASE(reqh[6]) == 'g' && TOLOWERCASE(reqh[7]) == 'e' && reqh[8] == ':')
        return If_Range;
    //If-Unmodified-Since
    else if(TOLOWERCASE(reqh[0]) == 'i' && TOLOWERCASE(reqh[1]) == 'f' && reqh[2] == '-' && TOLOWERCASE(reqh[3]) == 'u' && TOLOWERCASE(reqh[4]) == 'n' && TOLOWERCASE(reqh[5]) == 'm' && TOLOWERCASE(reqh[6]) == 'o' && TOLOWERCASE(reqh[7]) == 'd' && TOLOWERCASE(reqh[8]) == 'i'\
            && TOLOWERCASE(reqh[9]) == 'f' && TOLOWERCASE(reqh[10]) == 'i' && TOLOWERCASE(reqh[11]) == 'e' && TOLOWERCASE(reqh[12]) == 'd' && reqh[13] == '-' && TOLOWERCASE(reqh[14]) == 's' && TOLOWERCASE(reqh[15]) == 'i' && TOLOWERCASE(reqh[16]) == 'n'\
            && TOLOWERCASE(reqh[17]) == 'c' && TOLOWERCASE(reqh[18]) == 'e' && reqh[19] == ':')
        return If_Unmodified_Since;
    //Max-Forwards
    else if(TOLOWERCASE(reqh[0]) == 'm' && TOLOWERCASE(reqh[1]) == 'a' && TOLOWERCASE(reqh[3]) == 'x' && reqh[4] == '-' && TOLOWERCASE(reqh[5]) == 'f' && TOLOWERCASE(reqh[6]) == 'o' && TOLOWERCASE(reqh[7]) == 'r' && TOLOWERCASE(reqh[8]) == 'w'\
            && TOLOWERCASE(reqh[9]) == 'a' && TOLOWERCASE(reqh[10]) == 'r' && TOLOWERCASE(reqh[11]) == 'd' && TOLOWERCASE(reqh[12]) == 's' && reqh[13] == ':')
        return Max_Forwards;
    //Proxy-Authorization
    else if(TOLOWERCASE(reqh[0]) == 'p' && TOLOWERCASE(reqh[1]) == 'r' && TOLOWERCASE(reqh[2]) == 'o' && TOLOWERCASE(reqh[3]) == 'x' && TOLOWERCASE(reqh[4]) == 'y' && reqh[5] == '-'\
            && TOLOWERCASE(reqh[6]) == 'a' && TOLOWERCASE(reqh[7]) == 'u' &&TOLOWERCASE(reqh[8]) == 't' &&  TOLOWERCASE(reqh[9]) == 'h' && TOLOWERCASE(reqh[10]) == 'o' && TOLOWERCASE(reqh[11]) == 'r' && TOLOWERCASE(reqh[12]) == 'i' && TOLOWERCASE(reqh[13]) == 'z'\
            && TOLOWERCASE(reqh[14]) == 'a' && TOLOWERCASE(reqh[15]) == 't' && TOLOWERCASE(reqh[16]) == 'i' && TOLOWERCASE(reqh[17]) == 'o' && TOLOWERCASE(reqh[18]) == 'n' && reqh[19] == ':')
        return Proxy_Authorization;
    //Range
    else if(TOLOWERCASE(reqh[0]) == 'r' && TOLOWERCASE(reqh[1]) == 'a' && TOLOWERCASE(reqh[2]) == 'n' && TOLOWERCASE(reqh[3]) == 'g' && TOLOWERCASE(reqh[4]) == 'e' && reqh[5] == ':')
        return Range;
    //Referer
    else if(TOLOWERCASE(reqh[0]) == 'r' && TOLOWERCASE(reqh[1]) == 'e' && TOLOWERCASE(reqh[2]) == 'f' && TOLOWERCASE(reqh[3]) == 'e' && TOLOWERCASE(reqh[4]) == 'r' && TOLOWERCASE(reqh[5]) == 'e' && TOLOWERCASE(reqh[6]) == 'r' && reqh[7] == ':')
        return Referer;
    //TE
    else if(TOLOWERCASE(reqh[0]) == 't' && TOLOWERCASE(reqh[1]) == 'e' && reqh[2] == ':')
        return TE;
    //User-Agent
    else if(TOLOWERCASE(reqh[0]) == 'u' && TOLOWERCASE(reqh[1]) == 's' && TOLOWERCASE(reqh[2]) == 'e' && TOLOWERCASE(reqh[3]) == 'r' && reqh[4] == '-' && TOLOWERCASE(reqh[5]) == 'a' && TOLOWERCASE(reqh[6]) == 'g'\
            && TOLOWERCASE(reqh[7]) == 'e' && TOLOWERCASE(reqh[8]) == 'n' && TOLOWERCASE(reqh[9]) == 't' && reqh[10] == ':')
        return User_Agent;

    return INVALID_REQUEST_HEADER;

}
int main(int num, char ** args)
{
clock_t start, end;
double cpu_time_used;

char header1[]="Authorization:";
char header2[]="Accept-Language:";
char header3[]="Host:";
char wrong_header1[]="get";// wrong headers
char wrong_header2[]="Accept-";//wrong Accept-
http_request_header_t r1,r2,r3,r4,r5;
printf("header1: %s, header2: %s, header3: %s, header4: %s, header5: %s", header1, header2, header3, wrong_header1, wrong_header2);
start = clock();
for(int i=0; i < 10000; i++)
{
    r1=find_http_request_header_if_else(header1);
    r2=find_http_request_header_if_else(header2);
    r3=find_http_request_header_if_else(header3);
    r4=find_http_request_header_if_else(wrong_header1);
    r5=find_http_request_header_if_else(wrong_header2);

}
end = clock();
cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
printf("\nif-else parsing functions took: %f\n", cpu_time_used);
printf("results\n r1=%i,r2=%i,r3=%i,r4=%i,r5=%i\n",r1,r2,r3,r4,r5);



start = clock();
for(int i=0; i < 10000; i++)
{
    r1=find_http_request_header_switch_for(header1);
    r2=find_http_request_header_switch_for(header2);
    r3=find_http_request_header_switch_for(header3);
    r4=find_http_request_header_switch_for(wrong_header1);
    r5=find_http_request_header_switch_for(wrong_header2);

}
end = clock();
cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
printf("\nswitch-for parsing functions took: %f\n", cpu_time_used);
printf("results\n r1=%i,r2=%i,r3=%i,r4=%i,r5=%i\n",r1,r2,r3,r4,r5);

start = clock();
for(int i=0; i < 10000; i++)
{
    r1=find_http_request_header_switch(header1);
    r2=find_http_request_header_switch(header2);
    r3=find_http_request_header_switch(header3);
    r4=find_http_request_header_switch(wrong_header1);
    r5=find_http_request_header_switch(wrong_header2);

}
end = clock();
cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
printf("\nswitch parsing functions took: %f\n", cpu_time_used);
printf("results\n r1=%i,r2=%i,r3=%i,r4=%i,r5=%i\n",r1,r2,r3,r4,r5);
}
