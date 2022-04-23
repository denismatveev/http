#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "http.h"
#define TOLOWERCASE(c)      (char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define TOUPPERCASE(c)      (char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)
// This file created to compare different types for finding words matching using if-else statement and switch in case insensitive comparison.
// There is time calculation spent by different approaches. In the end consumed time by functions will be shown.
// These approaches has different perfomance if compile with no optimization(or default -O1 in gcc). But if specify -Ofast all functions works almost equally.
// To compile: gcc compare_case_insensitive.c -o compare_case_sensitive -Ofast
// The fastest turned out approach with switch-case. But between these two was approach3(switch-case with default action)
// Jump tables:
// http://www.cs.umd.edu/~waa/311-F09/jumptable.pdf
// disassemle objdump -d <file> -S <source>
static char* request_header[] =
{
    "accept:",
    "accept-charset:",
    "accept-encoding:",
    "accept-language:",
    "authorization:",
    "expect:",
    "from:",
    "host:",
    "if-match:",
    "if-modified-since:",
    "if-none-match:",
    "if-range:",
    "if-unmodified-since:",
    "max-forwards:",
    "proxy-authorization:",
    "range:",
    "referer:",
    "te:",
    "user-agent:",
    NULL
};
typedef struct __request_header_hash
{
    http_request_header_t h;
    unsigned long hash;
}request_header_hash;
static request_header_hash request_hash_arr[19]={0};
http_request_header_t find_http_request_header_app1(char *h)
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


http_request_header_t find_http_request_header_app2(char *reqh)
{

    // Accept
    if(TOLOWERCASE(reqh[0]) == 'a')
    {
        if (TOLOWERCASE(reqh[1]) == 'u' &&
                TOLOWERCASE(reqh[2]) == 't' &&
                TOLOWERCASE(reqh[3]) == 'h' &&
                TOLOWERCASE(reqh[4]) == 'o' &&
                TOLOWERCASE(reqh[5]) == 'r' &&
                TOLOWERCASE(reqh[6]) == 'i' &&
                TOLOWERCASE(reqh[7]) == 'z' &&
                TOLOWERCASE(reqh[8]) == 'a' &&
                TOLOWERCASE(reqh[9]) == 't' &&
                TOLOWERCASE(reqh[10]) == 'i' &&
                TOLOWERCASE(reqh[11]) == 'o' &&
                TOLOWERCASE(reqh[12]) == 'n' &&
                reqh[13] == ':')
            return Authorization;

        if
                (TOLOWERCASE(reqh[1]) == 'c' &&
                 TOLOWERCASE(reqh[2]) == 'c' &&
                 TOLOWERCASE(reqh[3]) == 'e' &&
                 TOLOWERCASE(reqh[4]) == 'p' &&
                 TOLOWERCASE(reqh[5]) == 't'
                 )
        {

            if (reqh[6] == ':')
                return Accept;
            //Accept-Charset
            if(
                    reqh[6] == '-' &&
                    TOLOWERCASE(reqh[7]) == 'c' &&
                    TOLOWERCASE(reqh[8]) == 'h' &&
                    TOLOWERCASE(reqh[9]) == 'a' &&
                    TOLOWERCASE(reqh[10]) == 'r' &&
                    TOLOWERCASE(reqh[11]) == 's' &&
                    TOLOWERCASE(reqh[12]) == 'e' &&
                    TOLOWERCASE(reqh[13]) == 't' &&
                    reqh[14] == ':'
                    )
                return Accept_Charset;
            //Accept-Encoding
            if(
                    reqh[6] == '-' &&
                    TOLOWERCASE(reqh[7]) == 'e' &&
                    TOLOWERCASE(reqh[8]) == 'n' &&
                    TOLOWERCASE(reqh[9]) == 'c' &&
                    TOLOWERCASE(reqh[10]) == 'o' &&
                    TOLOWERCASE(reqh[11]) == 'd' &&
                    TOLOWERCASE(reqh[12]) == 'i' &&
                    TOLOWERCASE(reqh[13]) == 'n' &&
                    TOLOWERCASE(reqh[14]) == 'g' &&
                    reqh[15] == ':'
                    )
                return Accept_Encoding;
            //Accept-Language
            if(
                    reqh[6] == '-' &&
                    TOLOWERCASE(reqh[7]) == 'l' &&
                    TOLOWERCASE(reqh[8]) == 'a' &&
                    TOLOWERCASE(reqh[9]) == 'n' &&
                    TOLOWERCASE(reqh[10]) == 'g' &&
                    TOLOWERCASE(reqh[11]) == 'u' &&
                    TOLOWERCASE(reqh[12]) == 'a' &&
                    TOLOWERCASE(reqh[13]) == 'g' &&
                    TOLOWERCASE(reqh[14]) == 'e' &&
                    reqh[15] == ':'
                    )
                return Accept_Language;
        }
    }

    //Expect
    if (TOLOWERCASE(reqh[0]) == 'e' &&
            TOLOWERCASE(reqh[1]) == 'x' &&
            TOLOWERCASE(reqh[2]) == 'p' &&
            TOLOWERCASE(reqh[3]) == 'e' &&
            TOLOWERCASE(reqh[4]) == 'c' &&
            TOLOWERCASE(reqh[5]) == 't' &&
            reqh[6] == ':')
        return Expect;
    //From
    if (TOLOWERCASE(reqh[0]) == 'f' &&
            TOLOWERCASE(reqh[1]) == 'r' &&
            TOLOWERCASE(reqh[2]) == 'o' &&
            TOLOWERCASE(reqh[3]) == 'm' &&
            reqh[4] == ':')
        return From;
    //Host
    if (TOLOWERCASE(reqh[0]) == 'h' &&
            TOLOWERCASE(reqh[1]) == 'o' &&
            TOLOWERCASE(reqh[2]) == 's' &&
            TOLOWERCASE(reqh[3]) == 't' &&
            reqh[4] == ':')
        return Host;

    if (TOLOWERCASE(reqh[0]) == 'i' &&
            TOLOWERCASE(reqh[1]) == 'f' &&
            reqh[2] == '-')
    {
        //If-Match
        if(
                TOLOWERCASE(reqh[3]) == 'm' &&
                TOLOWERCASE(reqh[4]) == 'a' &&
                TOLOWERCASE(reqh[5]) == 't' &&
                TOLOWERCASE(reqh[6]) == 'c' &&
                TOLOWERCASE(reqh[7]) == 'h' &&
                reqh[8] == ':')
            return If_Match;
        //If-Modified-Since
        if(
                TOLOWERCASE(reqh[3]) == 'm' &&
                TOLOWERCASE(reqh[4]) == 'o' &&
                TOLOWERCASE(reqh[5]) == 'd' &&
                TOLOWERCASE(reqh[6]) == 'i' &&
                TOLOWERCASE(reqh[7]) == 'f' &&
                TOLOWERCASE(reqh[8]) == 'i' &&
                TOLOWERCASE(reqh[9]) == 'e' &&
                TOLOWERCASE(reqh[10]) == 'd' &&
                reqh[11] == '-' &&
                TOLOWERCASE(reqh[12]) == 's' &&
                TOLOWERCASE(reqh[13]) == 'i' &&
                TOLOWERCASE(reqh[14]) == 'n' &&
                TOLOWERCASE(reqh[15]) == 'c' &&
                TOLOWERCASE(reqh[16]) == 'e' &&
                reqh[17] == ':')
            return If_Modified_Since;
        //If-None-Match
        if(
                TOLOWERCASE(reqh[3]) == 'n' &&
                TOLOWERCASE(reqh[4]) == 'o' &&
                TOLOWERCASE(reqh[5]) == 'n' &&
                TOLOWERCASE(reqh[6]) == 'e' &&
                reqh[7] == '-' &&
                TOLOWERCASE(reqh[8]) == 'm' &&
                TOLOWERCASE(reqh[9]) == 'a' &&
                TOLOWERCASE(reqh[10]) == 't' &&
                TOLOWERCASE(reqh[11]) == 'c' &&
                TOLOWERCASE(reqh[12]) == 'h' &&
                reqh[13] == ':')
            return If_None_Match;
        //If-Range
        if(
                TOLOWERCASE(reqh[3]) == 'r' &&
                TOLOWERCASE(reqh[4]) == 'a' &&
                TOLOWERCASE(reqh[5]) == 'n' &&
                TOLOWERCASE(reqh[6]) == 'g' &&
                TOLOWERCASE(reqh[7]) == 'e' &&
                reqh[8] == ':')
            return If_Range;
        //If-Unmodified-Since
        if(
                TOLOWERCASE(reqh[3]) == 'u' &&
                TOLOWERCASE(reqh[4]) == 'n' &&
                TOLOWERCASE(reqh[5]) == 'm' &&
                TOLOWERCASE(reqh[6]) == 'o' &&
                TOLOWERCASE(reqh[7]) == 'd' &&
                TOLOWERCASE(reqh[8]) == 'i' &&
                TOLOWERCASE(reqh[9]) == 'f' &&
                TOLOWERCASE(reqh[10]) == 'i' &&
                TOLOWERCASE(reqh[11]) == 'e' &&
                TOLOWERCASE(reqh[12]) == 'd' &&
                reqh[13] == '-' &&
                TOLOWERCASE(reqh[14]) == 's' &&
                TOLOWERCASE(reqh[15]) == 'i' &&
                TOLOWERCASE(reqh[16]) == 'n' &&
                TOLOWERCASE(reqh[17]) == 'c' &&
                TOLOWERCASE(reqh[18]) == 'e' &&
                reqh[19] == ':')
            return If_Unmodified_Since;
    }
    //Max-Forwards
    if(TOLOWERCASE(reqh[0]) == 'm' &&
            TOLOWERCASE(reqh[1]) == 'a' &&
            TOLOWERCASE(reqh[3]) == 'x' &&
            reqh[4] == '-' &&
            TOLOWERCASE(reqh[5]) == 'f' &&
            TOLOWERCASE(reqh[6]) == 'o' &&
            TOLOWERCASE(reqh[7]) == 'r' &&
            TOLOWERCASE(reqh[8]) == 'w' &&
            TOLOWERCASE(reqh[9]) == 'a' &&
            TOLOWERCASE(reqh[10]) == 'r' &&
            TOLOWERCASE(reqh[11]) == 'd' &&
            TOLOWERCASE(reqh[12]) == 's' &&
            reqh[13] == ':')
        return Max_Forwards;
    //Proxy-Authorization
    if(TOLOWERCASE(reqh[0]) == 'p' &&
            TOLOWERCASE(reqh[1]) == 'r' &&
            TOLOWERCASE(reqh[2]) == 'o' &&
            TOLOWERCASE(reqh[3]) == 'x' &&
            TOLOWERCASE(reqh[4]) == 'y' &&
            reqh[5] == '-' &&
            TOLOWERCASE(reqh[6]) == 'a' &&
            TOLOWERCASE(reqh[7]) == 'u' &&
            TOLOWERCASE(reqh[8]) == 't' &&
            TOLOWERCASE(reqh[9]) == 'h' &&
            TOLOWERCASE(reqh[10]) == 'o' &&
            TOLOWERCASE(reqh[11]) == 'r' &&
            TOLOWERCASE(reqh[12]) == 'i' &&
            TOLOWERCASE(reqh[13]) == 'z' &&
            TOLOWERCASE(reqh[14]) == 'a' &&
            TOLOWERCASE(reqh[15]) == 't' &&
            TOLOWERCASE(reqh[16]) == 'i' &&
            TOLOWERCASE(reqh[17]) == 'o' &&
            TOLOWERCASE(reqh[18]) == 'n' &&
            reqh[19] == ':')
        return Proxy_Authorization;
    //Range
    if(TOLOWERCASE(reqh[0]) == 'r' &&
            TOLOWERCASE(reqh[1]) == 'a' &&
            TOLOWERCASE(reqh[2]) == 'n' &&
            TOLOWERCASE(reqh[3]) == 'g' &&
            TOLOWERCASE(reqh[4]) == 'e' &&
            reqh[5] == ':')
        return Range;
    //Referer
    if(TOLOWERCASE(reqh[0]) == 'r' &&
            TOLOWERCASE(reqh[1]) == 'e' &&
            TOLOWERCASE(reqh[2]) == 'f' &&
            TOLOWERCASE(reqh[3]) == 'e' &&
            TOLOWERCASE(reqh[4]) == 'r' &&
            TOLOWERCASE(reqh[5]) == 'e' &&
            TOLOWERCASE(reqh[6]) == 'r' &&
            reqh[7] == ':')
        return Referer;
    //TE
    if(TOLOWERCASE(reqh[0]) == 't' &&
            TOLOWERCASE(reqh[1]) == 'e' &&
            reqh[2] == ':')
        return TE;
    //User-Agent
    if(TOLOWERCASE(reqh[0]) == 'u' &&
            TOLOWERCASE(reqh[1]) == 's' &&
            TOLOWERCASE(reqh[2]) == 'e' &&
            TOLOWERCASE(reqh[3]) == 'r' &&
            reqh[4] == '-' &&
            TOLOWERCASE(reqh[5]) == 'a' &&
            TOLOWERCASE(reqh[6]) == 'g' &&
            TOLOWERCASE(reqh[7]) == 'e' &&
            TOLOWERCASE(reqh[8]) == 'n' &&
            TOLOWERCASE(reqh[9]) == 't' &&
            reqh[10] == ':')
        return User_Agent;

    return INVALID_REQUEST_HEADER;

}
http_request_header_t find_http_request_header_app3(char *h)
{
    // this will be compiled as a jump table
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
                                                        default:
                                                            return INVALID_REQUEST_HEADER;
                                                        }
                                                    default:
                                                        return INVALID_REQUEST_HEADER;
                                                    }
                                                default:
                                                    return INVALID_REQUEST_HEADER;
                                                }
                                            default:
                                                return INVALID_REQUEST_HEADER;
                                            }
                                        default:
                                            return INVALID_REQUEST_HEADER;
                                        }
                                    default:
                                        return INVALID_REQUEST_HEADER;
                                    }
                                default:
                                    return INVALID_REQUEST_HEADER;
                                }
                            default:
                                return INVALID_REQUEST_HEADER;
                            }
                        default:
                            return INVALID_REQUEST_HEADER;
                        }
                    default:
                        return INVALID_REQUEST_HEADER;
                    }
                default:
                    return INVALID_REQUEST_HEADER;
                }
            default:
                return INVALID_REQUEST_HEADER;
            }

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
                                                            default:
                                                                return INVALID_REQUEST_HEADER;
                                                            }
                                                        default:
                                                            return INVALID_REQUEST_HEADER;
                                                        }
                                                    default:
                                                        return INVALID_REQUEST_HEADER;
                                                    }
                                                default:
                                                    return INVALID_REQUEST_HEADER;
                                                }
                                            default:
                                                return INVALID_REQUEST_HEADER;
                                            }
                                        default:
                                            return INVALID_REQUEST_HEADER;
                                        }
                                    default:
                                        return INVALID_REQUEST_HEADER;
                                    }
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
                                                                default:
                                                                    return INVALID_REQUEST_HEADER;
                                                                }
                                                            default:
                                                                return INVALID_REQUEST_HEADER;
                                                            }
                                                        default:
                                                            return INVALID_REQUEST_HEADER;
                                                        }
                                                    default:
                                                        return INVALID_REQUEST_HEADER;
                                                    }
                                                default:
                                                    return INVALID_REQUEST_HEADER;
                                                }
                                            default:
                                                return INVALID_REQUEST_HEADER;
                                            }
                                        default:
                                            return INVALID_REQUEST_HEADER;
                                        }
                                    default:
                                        return INVALID_REQUEST_HEADER;
                                    }
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
                                                                default:
                                                                    return INVALID_REQUEST_HEADER;
                                                                }
                                                            default:
                                                                return INVALID_REQUEST_HEADER;
                                                            }
                                                        default:
                                                            return INVALID_REQUEST_HEADER;
                                                        }
                                                    default:
                                                        return INVALID_REQUEST_HEADER;
                                                    }
                                                default:
                                                    return INVALID_REQUEST_HEADER;
                                                }
                                            default:
                                                return INVALID_REQUEST_HEADER;
                                            }
                                        default:
                                            return INVALID_REQUEST_HEADER;
                                        }
                                    default:
                                        return INVALID_REQUEST_HEADER;
                                    }
                                default:
                                    return INVALID_REQUEST_HEADER;
                                }
                            default:
                                return INVALID_REQUEST_HEADER;
                            }
                        default:
                            return INVALID_REQUEST_HEADER;
                        }
                    default:
                        return INVALID_REQUEST_HEADER;
                    }
                default:
                    return INVALID_REQUEST_HEADER;
                }
            default:
                return INVALID_REQUEST_HEADER;
            }
        default:
            return INVALID_REQUEST_HEADER;

        }
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
                            default:
                                return INVALID_REQUEST_HEADER;
                            }
                        default:
                            return INVALID_REQUEST_HEADER;
                        }
                    default:
                        return INVALID_REQUEST_HEADER;
                    }
                default:
                    return INVALID_REQUEST_HEADER;
                }
            default:
                return INVALID_REQUEST_HEADER;
            }
        default:
            return INVALID_REQUEST_HEADER;
        }
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
                    default:
                        return INVALID_REQUEST_HEADER;
                    }
                default:
                    return INVALID_REQUEST_HEADER;
                }
            default:
                return INVALID_REQUEST_HEADER;
            }
        default:
            return INVALID_REQUEST_HEADER;
        }
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
                    default:
                        return INVALID_REQUEST_HEADER;
                    }
                default:
                    return INVALID_REQUEST_HEADER;
                }
            default:
                return INVALID_REQUEST_HEADER;
            }
        default:
            return INVALID_REQUEST_HEADER;
        }
    case 'I':
    case 'i':
        switch(h[1])
        {// If-
        case 'F':
        case 'f':
            switch (h[2])
            {
            case '-':
                switch (h[3])
                {
                //"If-Match
                //"If-Modified-Since
                case 'M':
                case 'm':
                    switch (h[4])
                    {
                    case 'A':
                    case 'a':
                        switch(h[5])
                        {
                        case 'T':
                        case 't':
                            switch (h[6])
                            {
                            case 'C':
                            case 'c':
                                switch (h[7])
                                {
                                case ':':
                                    return If_Match;
                                default:
                                    return INVALID_REQUEST_HEADER;
                                }
                            default:
                                return INVALID_REQUEST_HEADER;
                            }
                        default:
                            return INVALID_REQUEST_HEADER;
                        }
                    case 'O':
                    case 'o':
                        switch (h[5])
                        {
                        case 'D':
                        case 'd':
                            switch (h[6])
                            {
                            case 'I':
                            case 'i':
                                switch (h[7])
                                {
                                case 'F':
                                case 'f':
                                    switch (h[8])
                                    {
                                    case 'I':
                                    case 'i':
                                        switch (h[9])
                                        {
                                        case 'E':
                                        case 'e':
                                            switch (h[10])
                                            {
                                            case 'D':
                                            case 'd':
                                                switch (h[11])
                                                {
                                                case '-':
                                                    switch (h[12])
                                                    {
                                                    case 'S':
                                                    case 's':
                                                        switch (h[13])
                                                        {
                                                        case 'I':
                                                        case 'i':
                                                            switch (h[14])
                                                            {
                                                            case 'N':
                                                            case 'n':
                                                                switch (h[15])
                                                                {
                                                                case 'C':
                                                                case 'c':
                                                                    switch (h[16])
                                                                    {
                                                                    case 'E':
                                                                    case 'e':
                                                                        switch (h[17])
                                                                        {
                                                                        case ':':
                                                                            return If_Modified_Since;
                                                                        default:
                                                                            return INVALID_REQUEST_HEADER;
                                                                        }
                                                                    default:
                                                                        return INVALID_REQUEST_HEADER;
                                                                    }
                                                                default:
                                                                    return INVALID_REQUEST_HEADER;
                                                                }
                                                            default:
                                                                return INVALID_REQUEST_HEADER;
                                                            }
                                                        default:
                                                            return INVALID_REQUEST_HEADER;
                                                        }
                                                    default:
                                                        return INVALID_REQUEST_HEADER;
                                                    }
                                                default:
                                                    return INVALID_REQUEST_HEADER;
                                                }
                                            default:
                                                return INVALID_REQUEST_HEADER;
                                            }
                                        default:
                                            return INVALID_REQUEST_HEADER;
                                        }
                                    default:
                                        return INVALID_REQUEST_HEADER;
                                    }
                                default:
                                    return INVALID_REQUEST_HEADER;
                                }
                            default:
                                return INVALID_REQUEST_HEADER;
                            }
                        default:
                            return INVALID_REQUEST_HEADER;
                        }
                    default:
                        return INVALID_REQUEST_HEADER;
                    }


                    //If-None-Match
                case 'N':
                case 'n':
                    switch (h[4])
                    {
                    case 'O':
                    case 'o':
                        switch (h[5])
                        {
                        case 'N':
                        case 'n':
                            switch (h[6])
                            {
                            case 'E':
                            case 'e':
                                switch (h[7])
                                {
                                case '-':
                                    switch (h[8])
                                    {
                                    case 'M':
                                    case 'm':
                                        switch (h[9])
                                        {
                                        case 'A':
                                        case 'a':
                                            switch (h[10])
                                            {
                                            case 'T':
                                            case 't':
                                                switch (h[11])
                                                {
                                                case 'C':
                                                case 'c':
                                                    switch (h[12])
                                                    {
                                                    case 'H':
                                                    case 'h':
                                                        switch (h[13])
                                                        {
                                                        case ':':
                                                            return If_None_Match;
                                                        default:
                                                            return INVALID_REQUEST_HEADER;
                                                        }
                                                    default:
                                                        return INVALID_REQUEST_HEADER;
                                                    }
                                                default:
                                                    return INVALID_REQUEST_HEADER;
                                                }
                                            default:
                                                return INVALID_REQUEST_HEADER;
                                            }
                                        default:
                                            return INVALID_REQUEST_HEADER;
                                        }
                                    default:
                                        return INVALID_REQUEST_HEADER;
                                    }
                                default:
                                    return INVALID_REQUEST_HEADER;
                                }
                            default:
                                return INVALID_REQUEST_HEADER;
                            }
                        default:
                            return INVALID_REQUEST_HEADER;
                        }
                    default:
                        return INVALID_REQUEST_HEADER;
                    }

                    //If-Range
                case 'R':
                case 'r':
                    switch (h[4])
                    {
                    case 'A':
                    case 'a':
                        switch (h[5])
                        {
                        case 'N':
                        case 'n':
                            switch (h[6])
                            {
                            case 'G':
                            case 'g':
                                switch (h[7])
                                {
                                case 'E':
                                case 'e':
                                    switch (h[8])
                                    {
                                    case ':':
                                        return If_Range;
                                    default:
                                        return INVALID_REQUEST_HEADER;
                                    }
                                default:
                                    return INVALID_REQUEST_HEADER;
                                }
                            default:
                                return INVALID_REQUEST_HEADER;
                            }
                        default:
                            return INVALID_REQUEST_HEADER;
                        }
                    default:
                        return INVALID_REQUEST_HEADER;
                    }

                    //If-Unmodified-Since
                case 'U':
                case 'u':
                    switch (h[4])
                    {
                    case 'N':
                    case 'n':
                        switch (h[5])
                        {
                        case 'M':
                        case 'n':
                            switch (h[6])
                            {
                            case 'O':
                            case 'o':
                                switch (h[7])
                                {
                                case 'D':
                                case 'd':
                                    switch (h[8])
                                    {
                                    case 'I':
                                    case 'i':
                                        switch (h[9])
                                        {
                                        case 'F':
                                        case 'f':
                                            switch (h[10])
                                            {
                                            case 'I':
                                            case 'i':
                                                switch (h[11])
                                                {
                                                case 'E':
                                                case 'e':
                                                    switch (h[12])
                                                    {
                                                    case 'D':
                                                    case 'd':
                                                        switch (h[13])
                                                        {
                                                        case '-':
                                                            switch (h[14])
                                                            {
                                                            case 'S':
                                                            case 's':
                                                                switch (h[15])
                                                                {
                                                                case 'I':
                                                                case 'i':
                                                                    switch (h[16])
                                                                    {
                                                                    case 'N':
                                                                    case 'n':
                                                                        switch (h[17])
                                                                        {
                                                                        case 'C':
                                                                        case 'c':
                                                                            switch (h[18])
                                                                            {
                                                                            case 'E':
                                                                            case 'e':
                                                                                switch (h[19])
                                                                                {
                                                                                case ':':
                                                                                    return If_Unmodified_Since;
                                                                                default:
                                                                                    return INVALID_REQUEST_HEADER;
                                                                                }
                                                                            default:
                                                                                return INVALID_REQUEST_HEADER;
                                                                            }
                                                                        default:
                                                                            return INVALID_REQUEST_HEADER;
                                                                        }
                                                                    default:
                                                                        return INVALID_REQUEST_HEADER;
                                                                    }
                                                                default:
                                                                    return INVALID_REQUEST_HEADER;
                                                                }
                                                            default:
                                                                return INVALID_REQUEST_HEADER;
                                                            }
                                                        default:
                                                            return INVALID_REQUEST_HEADER;
                                                        }
                                                    default:
                                                        return INVALID_REQUEST_HEADER;
                                                    }
                                                default:
                                                    return INVALID_REQUEST_HEADER;
                                                }
                                            default:
                                                return INVALID_REQUEST_HEADER;
                                            }
                                        default:
                                            return INVALID_REQUEST_HEADER;
                                        }
                                    default:
                                        return INVALID_REQUEST_HEADER;
                                    }
                                default:
                                    return INVALID_REQUEST_HEADER;
                                }
                            default:
                                return INVALID_REQUEST_HEADER;
                            }
                        default:
                            return INVALID_REQUEST_HEADER;
                        }
                    default:
                        return INVALID_REQUEST_HEADER;
                    }
                default:
                    return INVALID_REQUEST_HEADER;
                }
            default:
                return INVALID_REQUEST_HEADER;
            }
        default:
            return INVALID_REQUEST_HEADER;
        }
        //Max-Forwards
    case 'M':
    case 'm':
        switch (h[1])
        {
        case 'A':
        case 'a':
            switch (h[2])
            {
            case 'X':
            case 'x':
                switch (h[3])
                {
                case '-':
                    switch (h[4])
                    {
                    case 'F':
                    case 'f':
                        switch (h[5])
                        {
                        case 'O':
                        case 'o':
                            switch (h[6])
                            {
                            case 'R':
                            case 'r':
                                switch (h[7])
                                {
                                case 'W':
                                case 'w':
                                    switch (h[8])
                                    {
                                    case 'A':
                                    case 'a':
                                        switch (h[9])
                                        {
                                        case 'R':
                                        case 'r':
                                            switch (h[10])
                                            {
                                            case 'D':
                                            case 'd':
                                                switch (h[11])
                                                {
                                                case 'S':
                                                case 's':
                                                    switch (h[12])
                                                    {
                                                    case ':':
                                                        return Max_Forwards;
                                                    default:
                                                        return INVALID_REQUEST_HEADER;
                                                    }
                                                default:
                                                    return INVALID_REQUEST_HEADER;
                                                }
                                            default:
                                                return INVALID_REQUEST_HEADER;
                                            }
                                        default:
                                            return INVALID_REQUEST_HEADER;
                                        }
                                    default:
                                        return INVALID_REQUEST_HEADER;
                                    }
                                default:
                                    return INVALID_REQUEST_HEADER;
                                }
                            default:
                                return INVALID_REQUEST_HEADER;
                            }
                        default:
                            return INVALID_REQUEST_HEADER;
                        }
                    default:
                        return INVALID_REQUEST_HEADER;
                    }
                default:
                    return INVALID_REQUEST_HEADER;
                }
            default:
                return INVALID_REQUEST_HEADER;
            }
        default:
            return INVALID_REQUEST_HEADER;
        }

        //Proxy-Authorization
    case 'P':
    case 'p':
        switch (h[1])
        {
        case 'R':
        case 'r':
            switch (h[2])
            {
            case 'O':
            case 'o':
                switch (h[3])
                {
                case 'X':
                case 'x':
                    switch (h[4])
                    {
                    case 'Y':
                    case 'y':
                        switch (h[5])
                        {
                        case '-':
                            switch (h[6])
                            {
                            case 'A':
                            case 'a':
                                switch (h[7])
                                {
                                case 'U':
                                case 'u':
                                    switch (h[8])
                                    {
                                    case 'T':
                                    case 't':
                                        switch (h[9])
                                        {
                                        case 'H':
                                        case 'h':
                                            switch (h[10])
                                            {
                                            case 'O':
                                            case 'o':
                                                switch (h[11])
                                                {
                                                case 'R':
                                                case 'r':
                                                    switch (h[12])
                                                    {
                                                    case 'I':
                                                    case 'i':
                                                        switch (h[13])
                                                        {
                                                        case 'Z':
                                                        case 'z':
                                                            switch (h[14])
                                                            {
                                                            case 'A':
                                                            case 'a':
                                                                switch (h[15])
                                                                {
                                                                case 'T':
                                                                case 't':
                                                                    switch (h[16])
                                                                    {
                                                                    case 'I':
                                                                    case 'i':
                                                                        switch (h[17])
                                                                        {
                                                                        case 'O':
                                                                        case 'o':
                                                                            switch (h[18])
                                                                            {
                                                                            case 'N':
                                                                            case 'n':
                                                                                switch (h[19])
                                                                                {
                                                                                case ':':
                                                                                    return Proxy_Authorization;
                                                                                default:
                                                                                    return INVALID_REQUEST_HEADER;
                                                                                }
                                                                            default:
                                                                                return INVALID_REQUEST_HEADER;
                                                                            }
                                                                        default:
                                                                            return INVALID_REQUEST_HEADER;
                                                                        }
                                                                    default:
                                                                        return INVALID_REQUEST_HEADER;
                                                                    }
                                                                default:
                                                                    return INVALID_REQUEST_HEADER;
                                                                }
                                                            default:
                                                                return INVALID_REQUEST_HEADER;
                                                            }
                                                        default:
                                                            return INVALID_REQUEST_HEADER;
                                                        }
                                                    default:
                                                        return INVALID_REQUEST_HEADER;
                                                    }
                                                default:
                                                    return INVALID_REQUEST_HEADER;
                                                }
                                            default:
                                                return INVALID_REQUEST_HEADER;
                                            }
                                        default:
                                            return INVALID_REQUEST_HEADER;
                                        }
                                    default:
                                        return INVALID_REQUEST_HEADER;
                                    }
                                default:
                                    return INVALID_REQUEST_HEADER;
                                }
                            default:
                                return INVALID_REQUEST_HEADER;
                            }
                        default:
                            return INVALID_REQUEST_HEADER;
                        }
                    default:
                        return INVALID_REQUEST_HEADER;
                    }
                default:
                    return INVALID_REQUEST_HEADER;
                }
            default:
                return INVALID_REQUEST_HEADER;
            }
        default:
            return INVALID_REQUEST_HEADER;
        }

        //Range
        //Referer
    case 'R':
    case 'r':
        switch (h[1])
        {
        case 'A':
        case 'a':
            switch (h[2])
            {
            case 'N':
            case 'n':
                switch (h[3])
                {
                case 'G':
                case 'g':
                    switch (h[4])
                    {
                    case 'E':
                    case 'e':
                        switch (h[5])
                        {
                        case ':':
                            return Range;
                        default:
                            return INVALID_REQUEST_HEADER;
                        }
                    default:
                        return INVALID_REQUEST_HEADER;
                    }
                default:
                    return INVALID_REQUEST_HEADER;
                }
            default:
                return INVALID_REQUEST_HEADER;
            }

        case 'E':
        case 'e':
            switch (h[3])
            {
            case 'F':
            case 'f':
                switch (h[4])
                {
                case 'E':
                case 'e':
                    switch (h[5])
                    {
                    case 'R':
                    case 'r':
                        switch (h[6])
                        {
                        case 'E':
                        case 'e':
                            switch (h[7])
                            {
                            case 'R':
                            case 'r':
                                switch (h[8])
                                {
                                case ':':
                                    return Referer;
                                default:
                                    return INVALID_REQUEST_HEADER;
                                }
                            default:
                                return INVALID_REQUEST_HEADER;
                            }
                        default:
                            return INVALID_REQUEST_HEADER;
                        }
                    default:
                        return INVALID_REQUEST_HEADER;
                    }
                default:
                    return INVALID_REQUEST_HEADER;
                }
            default:
                return INVALID_REQUEST_HEADER;
            }
        default:
            return INVALID_REQUEST_HEADER;
        }
        //TE
    case 'T':
    case 't':
        switch (h[1])
        {
        case 'E':
        case 'e':
            switch (h[2])
            {
            case ':':
                return TE;
            default:
                return INVALID_REQUEST_HEADER;
            }
        default:
            return INVALID_REQUEST_HEADER;
        }
        //User-Agent
    case 'U':
    case 'u':
        switch (h[1])
        {
        case 'S':
        case 's':
            switch (h[2])
            {
            case 'E':
            case 'e':
                switch (h[3])
                {
                case 'R':
                case 'r':
                    switch (h[4])
                    {
                    case '-':
                        switch (h[5])
                        {
                        case 'A':
                        case 'a':
                            switch (h[6])
                            {
                            case 'G':
                            case 'g':
                                switch (h[7])
                                {
                                case 'E':
                                case 'e':
                                    switch (h[8])
                                    {
                                    case 'N':
                                    case 'n':
                                        switch (h[9])
                                        {
                                        case 'T':
                                        case 't':
                                            switch (h[10])
                                            {
                                            case ':':
                                                return User_Agent;
                                            default:
                                                return INVALID_REQUEST_HEADER;
                                            }
                                        default:
                                            return INVALID_REQUEST_HEADER;
                                        }
                                    default:
                                        return INVALID_REQUEST_HEADER;
                                    }
                                default:
                                    return INVALID_REQUEST_HEADER;
                                }
                            default:
                                return INVALID_REQUEST_HEADER;
                            }
                        default:
                            return INVALID_REQUEST_HEADER;
                        }
                    default:
                        return INVALID_REQUEST_HEADER;
                    }
                default:
                    return INVALID_REQUEST_HEADER;
                }
            default:
                return INVALID_REQUEST_HEADER;
            }
        default:
            return INVALID_REQUEST_HEADER;
        }
    default:
        return INVALID_REQUEST_HEADER;
    }

}

http_request_header_t find_http_request_header_app4(char *h)
{
    // this will be compiled as a jump table
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
    case 'I':
    case 'i':
        switch(h[1])
        {// If-
        case 'F':
        case 'f':
            switch (h[2])
            {
            case '-':
                switch (h[3])
                {
                //"If-Match
                //"If-Modified-Since
                case 'M':
                case 'm':
                    switch (h[4])
                    {
                    case 'A':
                    case 'a':
                        switch(h[5])
                        {
                        case 'T':
                        case 't':
                            switch (h[6])
                            {
                            case 'C':
                            case 'c':
                                switch (h[7])
                                {
                                case ':':
                                    return If_Match;
                                }
                                break;
                            }
                            break;
                        }
                        break;
                    case 'O':
                    case 'o':
                        switch (h[5])
                        {
                        case 'D':
                        case 'd':
                            switch (h[6])
                            {
                            case 'I':
                            case 'i':
                                switch (h[7])
                                {
                                case 'F':
                                case 'f':
                                    switch (h[8])
                                    {
                                    case 'I':
                                    case 'i':
                                        switch (h[9])
                                        {
                                        case 'E':
                                        case 'e':
                                            switch (h[10])
                                            {
                                            case 'D':
                                            case 'd':
                                                switch (h[11])
                                                {
                                                case '-':
                                                    switch (h[12])
                                                    {
                                                    case 'S':
                                                    case 's':
                                                        switch (h[13])
                                                        {
                                                        case 'I':
                                                        case 'i':
                                                            switch (h[14])
                                                            {
                                                            case 'N':
                                                            case 'n':
                                                                switch (h[15])
                                                                {
                                                                case 'C':
                                                                case 'c':
                                                                    switch (h[16])
                                                                    {
                                                                    case 'E':
                                                                    case 'e':
                                                                        switch (h[17])
                                                                        {
                                                                        case ':':
                                                                            return If_Modified_Since;
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


                    //If-None-Match
                case 'N':
                case 'n':
                    switch (h[4])
                    {
                    case 'O':
                    case 'o':
                        switch (h[5])
                        {
                        case 'N':
                        case 'n':
                            switch (h[6])
                            {
                            case 'E':
                            case 'e':
                                switch (h[7])
                                {
                                case '-':
                                    switch (h[8])
                                    {
                                    case 'M':
                                    case 'm':
                                        switch (h[9])
                                        {
                                        case 'A':
                                        case 'a':
                                            switch (h[10])
                                            {
                                            case 'T':
                                            case 't':
                                                switch (h[11])
                                                {
                                                case 'C':
                                                case 'c':
                                                    switch (h[12])
                                                    {
                                                    case 'H':
                                                    case 'h':
                                                        switch (h[13])
                                                        {
                                                        case ':':
                                                            return If_None_Match;

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

                    //If-Range
                case 'R':
                case 'r':
                    switch (h[4])
                    {
                    case 'A':
                    case 'a':
                        switch (h[5])
                        {
                        case 'N':
                        case 'n':
                            switch (h[6])
                            {
                            case 'G':
                            case 'g':
                                switch (h[7])
                                {
                                case 'E':
                                case 'e':
                                    switch (h[8])
                                    {
                                    case ':':
                                        return If_Range;
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

                    //If-Unmodified-Since
                case 'U':
                case 'u':
                    switch (h[4])
                    {
                    case 'N':
                    case 'n':
                        switch (h[5])
                        {
                        case 'M':
                        case 'n':
                            switch (h[6])
                            {
                            case 'O':
                            case 'o':
                                switch (h[7])
                                {
                                case 'D':
                                case 'd':
                                    switch (h[8])
                                    {
                                    case 'I':
                                    case 'i':
                                        switch (h[9])
                                        {
                                        case 'F':
                                        case 'f':
                                            switch (h[10])
                                            {
                                            case 'I':
                                            case 'i':
                                                switch (h[11])
                                                {
                                                case 'E':
                                                case 'e':
                                                    switch (h[12])
                                                    {
                                                    case 'D':
                                                    case 'd':
                                                        switch (h[13])
                                                        {
                                                        case '-':
                                                            switch (h[14])
                                                            {
                                                            case 'S':
                                                            case 's':
                                                                switch (h[15])
                                                                {
                                                                case 'I':
                                                                case 'i':
                                                                    switch (h[16])
                                                                    {
                                                                    case 'N':
                                                                    case 'n':
                                                                        switch (h[17])
                                                                        {
                                                                        case 'C':
                                                                        case 'c':
                                                                            switch (h[18])
                                                                            {
                                                                            case 'E':
                                                                            case 'e':
                                                                                switch (h[19])
                                                                                {
                                                                                case ':':
                                                                                    return If_Unmodified_Since;
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
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;
        //Max-Forwards
    case 'M':
    case 'm':
        switch (h[1])
        {
        case 'A':
        case 'a':
            switch (h[2])
            {
            case 'X':
            case 'x':
                switch (h[3])
                {
                case '-':
                    switch (h[4])
                    {
                    case 'F':
                    case 'f':
                        switch (h[5])
                        {
                        case 'O':
                        case 'o':
                            switch (h[6])
                            {
                            case 'R':
                            case 'r':
                                switch (h[7])
                                {
                                case 'W':
                                case 'w':
                                    switch (h[8])
                                    {
                                    case 'A':
                                    case 'a':
                                        switch (h[9])
                                        {
                                        case 'R':
                                        case 'r':
                                            switch (h[10])
                                            {
                                            case 'D':
                                            case 'd':
                                                switch (h[11])
                                                {
                                                case 'S':
                                                case 's':
                                                    switch (h[12])
                                                    {
                                                    case ':':
                                                        return Max_Forwards;
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

        //Proxy-Authorization
    case 'P':
    case 'p':
        switch (h[1])
        {
        case 'R':
        case 'r':
            switch (h[2])
            {
            case 'O':
            case 'o':
                switch (h[3])
                {
                case 'X':
                case 'x':
                    switch (h[4])
                    {
                    case 'Y':
                    case 'y':
                        switch (h[5])
                        {
                        case '-':
                            switch (h[6])
                            {
                            case 'A':
                            case 'a':
                                switch (h[7])
                                {
                                case 'U':
                                case 'u':
                                    switch (h[8])
                                    {
                                    case 'T':
                                    case 't':
                                        switch (h[9])
                                        {
                                        case 'H':
                                        case 'h':
                                            switch (h[10])
                                            {
                                            case 'O':
                                            case 'o':
                                                switch (h[11])
                                                {
                                                case 'R':
                                                case 'r':
                                                    switch (h[12])
                                                    {
                                                    case 'I':
                                                    case 'i':
                                                        switch (h[13])
                                                        {
                                                        case 'Z':
                                                        case 'z':
                                                            switch (h[14])
                                                            {
                                                            case 'A':
                                                            case 'a':
                                                                switch (h[15])
                                                                {
                                                                case 'T':
                                                                case 't':
                                                                    switch (h[16])
                                                                    {
                                                                    case 'I':
                                                                    case 'i':
                                                                        switch (h[17])
                                                                        {
                                                                        case 'O':
                                                                        case 'o':
                                                                            switch (h[18])
                                                                            {
                                                                            case 'N':
                                                                            case 'n':
                                                                                switch (h[19])
                                                                                {
                                                                                case ':':
                                                                                    return Proxy_Authorization;
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
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;

        //Range
        //Referer
    case 'R':
    case 'r':
        switch (h[1])
        {
        case 'A':
        case 'a':
            switch (h[2])
            {
            case 'N':
            case 'n':
                switch (h[3])
                {
                case 'G':
                case 'g':
                    switch (h[4])
                    {
                    case 'E':
                    case 'e':
                        switch (h[5])
                        {
                        case ':':
                            return Range;
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
            switch (h[3])
            {
            case 'F':
            case 'f':
                switch (h[4])
                {
                case 'E':
                case 'e':
                    switch (h[5])
                    {
                    case 'R':
                    case 'r':
                        switch (h[6])
                        {
                        case 'E':
                        case 'e':
                            switch (h[7])
                            {
                            case 'R':
                            case 'r':
                                switch (h[8])
                                {
                                case ':':
                                    return Referer;
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
        //TE
    case 'T':
    case 't':
        switch (h[1])
        {
        case 'E':
        case 'e':
            switch (h[2])
            {
            case ':':
                return TE;
            }
            break;
        }
        break;
        //User-Agent
    case 'U':
    case 'u':
        switch (h[1])
        {
        case 'S':
        case 's':
            switch (h[2])
            {
            case 'E':
            case 'e':
                switch (h[3])
                {
                case 'R':
                case 'r':
                    switch (h[4])
                    {
                    case '-':
                        switch (h[5])
                        {
                        case 'A':
                        case 'a':
                            switch (h[6])
                            {
                            case 'G':
                            case 'g':
                                switch (h[7])
                                {
                                case 'E':
                                case 'e':
                                    switch (h[8])
                                    {
                                    case 'N':
                                    case 'n':
                                        switch (h[9])
                                        {
                                        case 'T':
                                        case 't':
                                            switch (h[10])
                                            {
                                            case ':':
                                                return User_Agent;
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

    return INVALID_REQUEST_HEADER;

}
http_request_header_t find_http_request_header_app5(char* v)
{
    int j = 0, i = 0;

    for (i = 0; request_header[j] != NULL && v[i] != ':';)
    {
        if(TOLOWERCASE(v[i]) != request_header[j][i])
            j++;
        else i++;

    }
    return (v[i] == ':' ? j :  INVALID_REQUEST_HEADER);

}

unsigned long int hash_func_app6(char *k)
{

    unsigned long int hash = 5381;
    char s=0;
    int i=0;

    while ((s = k[i++]))
        hash = ((hash << 5) + hash) + (unsigned long)TOLOWERCASE(s); /* hash * 33 + c */

    return hash;
}

int init_hash_table_app6(char* request_header[])
{
    unsigned long int hash;
    int i = 0;
    while(request_header[i] != NULL)
    {
        hash = hash_func_app6(request_header[i]);
        request_hash_arr[i].hash = hash;
        request_hash_arr[i].h = i;
        i++;
    }
    return 0;
}

//TODO compare 1st letter, then calculate hash of header which begins from that letter, compare hashes. Let's say, the first letter is h, calculate hash for header 'host', compare hash with precalculated hash.
http_request_header_t find_http_request_header_app6(char* v)
{
    unsigned long int hash;

    hash = hash_func_app6(v);
    //    "accept:", 0
    //    "accept-charset:", 1
    //    "accept-encoding:", 2
    //    "accept-language:", 3
    //    "authorization:", 4
    //    "expect:", 5
    //    "from:", 6
    //    "host:", 7
    //    "if-match:", 8
    //    "if-modified-since:", 9
    //    "if-none-match:", 10
    //    "if-range:", 11
    //    "if-unmodified-since:", 12
    //    "max-forwards:", 13
    //    "proxy-authorization:", 14
    //    "range:", 15
    //    "referer:", 16
    //    "te:", 17
    //    "user-agent:", 18
    // TODO
    // it is impossible to compare non-integer in switch
    // The following implementation with branching by the first letter works a bit faster than using pure if-else statements
    switch (v[0])
    {
    case 'A':
    case 'a':
        if(hash == request_hash_arr[0].hash)
            return request_hash_arr[0].h;
        if(hash == request_hash_arr[1].hash)
            return request_hash_arr[1].h;
        if(hash == request_hash_arr[2].hash)
            return request_hash_arr[2].h;
        if(hash == request_hash_arr[3].hash)
            return request_hash_arr[3].h;
        if(hash == request_hash_arr[4].hash)
            return request_hash_arr[4].h;
        return INVALID_REQUEST_HEADER;

    case 'E':
    case 'e':
        if(hash == request_hash_arr[5].hash)
            return request_hash_arr[5].h;
        return INVALID_REQUEST_HEADER;

    case 'F':
    case 'f':
        if(hash == request_hash_arr[6].hash)
            return request_hash_arr[6].h;
        return INVALID_REQUEST_HEADER;

    case 'H':
    case 'h':
        if(hash == request_hash_arr[7].hash)
            return request_hash_arr[7].h;
        return INVALID_REQUEST_HEADER;

    case 'I':
    case 'i':
        if(hash == request_hash_arr[8].hash)
            return request_hash_arr[8].h;
        if(hash == request_hash_arr[9].hash)
            return request_hash_arr[9].h;
        if(hash == request_hash_arr[10].hash)
            return request_hash_arr[10].h;
        if(hash == request_hash_arr[11].hash)
            return request_hash_arr[11].h;
        if(hash == request_hash_arr[12].hash)
            return request_hash_arr[12].h;
        return INVALID_REQUEST_HEADER;

    case 'M':
    case 'm':
        if(hash == request_hash_arr[13].hash)
            return request_hash_arr[13].h;
        return INVALID_REQUEST_HEADER;

    case 'P':
    case 'p':
        if(hash == request_hash_arr[14].hash)
            return request_hash_arr[14].h;
        return INVALID_REQUEST_HEADER;

    case 'R':
    case 'r':
        if(hash == request_hash_arr[15].hash)
            return request_hash_arr[15].h;
        if(hash == request_hash_arr[16].hash)
            return request_hash_arr[16].h;
        return INVALID_REQUEST_HEADER;

    case 'T':
    case 't':
        if(hash == request_hash_arr[17].hash)
            return request_hash_arr[17].h;
        return INVALID_REQUEST_HEADER;

    case 'U':
    case 'u':
        if(hash == request_hash_arr[18].hash)
            return request_hash_arr[18].h;
        return INVALID_REQUEST_HEADER;

    default:
        return INVALID_REQUEST_HEADER;

    }


}


//TODO to prevent using collisions resolving, find perfect hash function
int calculate_func_time(http_request_header_t (*app_func)(char* h), char* description)
{
    clock_t start, end;
    double cpu_time_used;
    http_request_header_t r1,r2,r3,r4,r5,r6,r7,r8;
    char h1[]="Host:";
    char h2[]="User-agent:";
    char h3[]="Proxy-Authorization:";
    char h4[]=""; // wrong
    char h5[]="From:";
    char h6[]="Accept-Encoding:";
    char h7[]="abc";// wrong
    char h8[]="Host"; // wrong
    start = clock();
    for(int i=0; i < 10000; i++)
    {

        r1=app_func(h1);
        r2=app_func(h2);
        r3=app_func(h3);
        r4=app_func(h4);
        r5=app_func(h5);
        r6=app_func(h6);
        r7=app_func(h7);
        r8=app_func(h8);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("r1=%i|r2=%i|r3=%i|r4=%i|r5=%i|r6=%i|r7=%i|r8=%i|time=%f",r1,r2,r3,r4,r5,r6,r7,r8, cpu_time_used);
    printf("\t%s\n",description);
    return 0;

}
int main (int argc, char** argv)
{
    init_hash_table_app6(request_header);
    calculate_func_time(find_http_request_header_app1, "approach 1(switch with splitting into groups, then 'for' loop and strncmp)");
    calculate_func_time(find_http_request_header_app2, "approach 2(compare each letter in lower case with no loops)");
    calculate_func_time(find_http_request_header_app3, "approach 3(switch and break-default action if no matching)");
    calculate_func_time(find_http_request_header_app4, "approach 4(switch and return with no default action if no matching)");
    calculate_func_time(find_http_request_header_app5, "approach 5(for-if searching matching, but without rolling back like strncasecmp() function");
    calculate_func_time(find_http_request_header_app6, "approach 6(hash tables)");
}
