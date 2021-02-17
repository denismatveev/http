#include<stdio.h>
#include<time.h>
#include "../http.h"

http_request_header_t find_http_request_header(const char *h)
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
        switch (h[2])
        {
        case 'S':
        case 's':
            switch (h[3])
            {
            case 'E':
            case 'e':
                switch (h[4])
                {
                case 'R':
                case 'r':
                    switch (h[5])
                    {
                    case '-':
                        switch (h[6])
                        {
                        case 'A':
                        case 'a':
                            switch (h[7])
                            {
                            case 'G':
                            case 'g':
                                switch (h[8])
                                {
                                case 'E':
                                case 'e':
                                    switch (h[9])
                                    {
                                    case 'N':
                                    case 'n':
                                        switch (h[10])
                                        {
                                        case 'T':
                                        case 't':
                                            switch (h[11])
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
int main (int argc, char** argv)
{
    clock_t start, end;
    double cpu_time_used;
    char h1[]="Host:";
    char h2[]="User-agent:";
    char h3[]="Proxy-Authorization:";
    char h4[]=""; // wrong
    char h5[]="From:";
    char h6[]="Accept-Encoding:";
    char h7[]="abc";// wrong
    char h8[]="Host"; // wrong
    http_request_header_t r1,r2,r3,r4,r5,r6,r7,r8;


    start = clock();
    for(int i=0; i < 10000; i++)
    {
        r1=find_http_request_header(h1);
        r2=find_http_request_header(h2);
        r3=find_http_request_header(h3);
        r4=find_http_request_header(h4);
        r5=find_http_request_header(h5);
        r6=find_http_request_header(h6);
        r7=find_http_request_header(h7);
        r8=find_http_request_header(h8);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\nswitch1 parsing functions took: %f\n", cpu_time_used);
    printf("results\n r1=%i,r2=%i,r3=%i,r4=%i,r5=%i, r6=%i, r7=%i, r8=%i\n",r1,r2,r3,r4,r5,r6,r7,r8);
}
