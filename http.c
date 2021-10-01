#include "http.h"
#include <locale.h>
#include <time.h>
// Based on RFC 2616
// https://datatracker.ietf.org/doc/html/rfc2616

/* General */

 static const char *http_protocol_version[] =
{
#define XX(num, name, proto_str) #proto_str,
HTTP_PROTOCOL(XX)
#undef XX
};
static char *reason_code_name[] =
{
#define XX(num, name, reason_code_str) #reason_code_str,
HTTP_REASON_CODE(XX)
#undef XX
};

static char *content_type[] =
{
    "text/html",
    "image/jpg",
    "application/pdf",
    "image/png",
    "video/mpeg",
    "text/css",
    NULL
};
static char *file_ext[] = {".html", ".jpg", ".pdf", ".png", ".mpeg", ".css", NULL};
// the following is used in both request and response
static char* general_header[] =
{
    "Cache-Control:",
    "Connection:",
    "Date:",
    "Pragma:",
    "Trailer:",
    "Transfer-Encoding:",
    "Upgrade:",
    "Via:",
    "Warning:",
    NULL
};
// the following is used in response and therefore contains colon and space
static char* response_header[] =
{
    "Accept-Ranges:",
    "Age:",
    "ETag:",
    "Location:",
    "Proxy-Authenticate:",
    "Retry-After:",
    "Server:",
    "Vary:",
    "WWW-Authenticate:",
    NULL
};
// the following is used in both request and response
static char* entity_header[] =
{
    "Allow:",
    "Content-Encoding:",
    "Content-Language:",
    "Content-Length:",
    "Content-Location:",
    "Content-MD5:",
    "Content-Range:",
    "Content-Type:",
    "Expires:",
    "Last-Modified:",
    NULL
};

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
static char* http_method[] =
{
    "GET",
    "POST",
    "HEAD",
    "PUT",
    "DELETE",
    "CONNECT",
    "OPTIONS",
    "TRACE",
    "PATCH",
    NULL
};
// errors

static char* bad_request_error = "<html><body><head><title>400 Bad Request</title></head><center><h1> 400 Bad Request <h1></center></body></html>";
static char* not_found_error = "<html><body><head><titile>404 Not Found</title></head><center><h1> 404 Not Found <h1></center></body></html>";
static char* internal_error = "<html><body><head><titile>500 Internal Error</title></head><center><h1> 500 Internal Error <h1></center></body></html>";
static char* not_implemented_error = "<html><body><head><titile>501 Not Implemented</title></head><center><h1> 501 Not Implemented <h1></center></body></html>";

// According to RFC2616 a method must be case sensitive
// Implied the passing string is null terminated, but doesn't verified if there is '/0' at the end position
// A caller must take care about null terminating

http_method_t str_to_http_method(const char *sval)
{
    http_method_t result=INVALID_METHOD;
    switch (sval[0])
    {
    case 'G':
        // GET
        switch(sval[1])
        {
        case 'E':
            switch (sval[2])
            {
            case 'T':
                result = GET;
                break;
            default:
                return INVALID_METHOD;
            }
            break;
        default:
            return INVALID_METHOD;
        }
        break;
    case 'P':
        // POST, PUT, PATCH
        switch(sval[1])
        {
        case 'O':
            // POST
            switch(sval[2])
            {
            case 'S':
                switch(sval[3])
                {
                case 'T':
                    result = POST;
                    break;
                default:
                    return INVALID_METHOD;
                }
                break;
            default:
                return INVALID_METHOD;
            }
            break;

        case 'U':
            //PUT
            switch (sval[2])
            {
            case 'T':
                result = PUT;
                break;
            default:
                return INVALID_METHOD;
            }
            break;
        case 'A':
            //PATCH
            switch(sval[2])
            {
            case 'T':
                switch(sval[3])
                {
                case 'C':
                    switch(sval[4])
                    {
                    case 'H':
                        result = PATCH;
                        break;
                    default:
                        return INVALID_METHOD;
                    }
                    break;
                default:
                    return INVALID_METHOD;
                }
                break;
            default:
                return INVALID_METHOD;
            }
            break;
        default:
            return INVALID_METHOD;
        }
        break;
    case 'H':
        // HEAD
        switch(sval[1])
        {
        case 'E':
            switch(sval[2])
            {
            case 'A':
                switch(sval[3])
                {
                case 'D':
                    result = HEAD;
                    break;
                default:
                    return INVALID_METHOD;
                }
                break;
            default:
                return INVALID_METHOD;
            }
            break;
        default:
            return INVALID_METHOD;
        }
        break;
    case 'D':
        // DELETE
        switch (sval[1])
        {
        case 'E':
            switch(sval[2])
            {
            case 'L':
                switch(sval[3])
                {
                case 'E':
                    switch(sval[4])
                    {
                    case 'T':
                        switch(sval[5])
                        {
                        case 'E':
                            result = DELETE;
                            break;
                        }
                        break;
                    default:
                        return INVALID_METHOD;
                    }
                    break;
                default:
                    return INVALID_METHOD;

                }
                break;
            default:
                return INVALID_METHOD;
            }
            break;
        default:
            return INVALID_METHOD;
        }
        break;
    case 'C':
        // CONNECT
        switch (sval[1])
        {
        case 'O':
            switch (sval[2])
            {
            case 'N':
                switch (sval[3])
                {
                case 'N':
                    switch (sval[4])
                    {
                    case 'E':
                        switch (sval[5])
                        {
                        case 'C':
                            switch (sval[6])
                            {
                            case 'T':
                                result = CONNECT;
                                break;
                            }
                            break;
                        default:
                            return INVALID_METHOD;
                        }
                        break;
                    default:
                        return INVALID_METHOD;
                    }
                    break;
                default:
                    return INVALID_METHOD;
                }
                break;
            default:
                return INVALID_METHOD;
            }
            break;
        default:
            return INVALID_METHOD;
        }
        break;
    case 'O':
        // OPTIONS
        switch (sval[1])
        {
        case 'P':
            switch (sval[2])
            {
            case 'T':
                switch (sval[3])
                {
                case 'I':
                    switch (sval[4])
                    {
                    case 'O':
                        switch (sval[5])
                        {
                        case 'N':
                            switch (sval[6])
                            {
                            case 'S':
                                result = OPTIONS;
                                break;
                            default:
                                return INVALID_METHOD;
                            }
                            break;
                        default:
                            return INVALID_METHOD;
                        }
                        break;
                    default:
                        return INVALID_METHOD;
                    }
                    break;
                default:
                    return INVALID_METHOD;
                }
                break;
            default:
                return INVALID_METHOD;
            }
            break;
        default:
            return INVALID_METHOD;
        }
        break;
    case 'T':
        // TRACE
        switch(sval[1])
        {
        case 'R':
            switch (sval[2])
            {
            case 'A':
                switch (sval[3])
                {
                case 'C':
                    switch (sval[4])
                    {
                    case 'E':
                        result = TRACE;
                        break;
                    default:
                        return INVALID_METHOD;
                    }
                    break;
                default:
                    return INVALID_METHOD;
                }
                break;
            default:
                return INVALID_METHOD;
            }
            break;
        default:
            return INVALID_METHOD;
        }
        break;
    default:
        return INVALID_METHOD;
    }

    return result;
}



http_method_t str_to_method2(const char *sval)
{


}


http_protocol_version_t str_to_http_protocol(const char *sval)
{

    http_protocol_version_t result = INVALID_PROTO;
    switch (sval[0])
    {
    case 'H':
        switch (sval[1])
        {
        case 'T':
            switch (sval[2])
            {
            case 'T':
                switch (sval[3])
                {
                case 'P':
                    switch (sval[4])
                    {
                    case '/':
                        switch (sval[5])
                        {
                        case '1':
                            switch (sval[6])
                            {
                            case '.':
                                switch (sval[7])
                                {
                                case '0':
                                    result = PROTO_HTTP10;
                                    break;
                                case '1':
                                    result = PROTO_HTTP11;
                                    break;
                                default:
                                    return INVALID_PROTO;
                                }
                                break;
                            default:
                                return INVALID_PROTO;
                            }
                            break;
                        default:
                            return INVALID_PROTO;
                        }
                        break;
                    default:
                        return INVALID_PROTO;
                    }
                    break;
                default:
                    return INVALID_PROTO;
                }
                break;
            default:
                return INVALID_PROTO;
            }
            break;
        default:
            return INVALID_PROTO;
        }
        break;
    default:
        return INVALID_PROTO;
    }

    return result;
}

// convert content_type into string
// It is for supported MIMEs, you must ajust CONTENT_TYPE_MAX_LENGTH for new implemented types

int content_type_to_str(char *str, http_content_type_t ct, unsigned char str_len)
{
    if(str_len < CONTENT_TYPE_MAX_LENGTH)
        return 2;
    switch (ct)
    {
    case text_html:
        strncpy(str,content_type[0], str_len);
        break;
    case image_jpg:
        strncpy(str,content_type[1], str_len);
        break;
    case application_pdf:
        strncpy(str,content_type[2], str_len);
        break;
    case image_png:
        strncpy(str,content_type[3], str_len);
        break;
    case video_mpeg:
        strncpy(str,content_type[4], str_len);
        break;
    case text_css:
        strncpy(str,content_type[5], str_len);
        break;
    default:
        return 1;
    }
    return 0;
}
// converts http code enumeration into c string
int http_ptorocol_code_to_str(char *str, http_protocol_version_t rt, unsigned char str_len)
{
    if(str_len < HTTP_PROTOCOL_VERSION_MAX_LENGTH)
        return 2;
    switch(rt)
    {
    case PROTO_HTTP10:
        strncpy(str, http_protocol_version[0], str_len);
        break;
    case PROTO_HTTP11:
        strncpy(str, http_protocol_version[1], str_len);
        break;
    default :
        return 1;
    }
    return 0;
}

// convert reason code into string, ie 404 -> Not Found
// maximum length is 17 for supported codes, you must ajust if implemented new

int reason_code_to_str(char *str, http_reason_code_t rt, unsigned char str_len)
{
    if(str_len < REASON_CODE_NAME_MAX_LENGTH)
        return 2;

    switch(rt)
    {
    case REASON_BAD_REQUEST:
        strncpy(str, reason_code_name[0], str_len);
        break;
    case REASON_NOT_FOUND:
        strncpy(str,reason_code_name[1], str_len);
        break;
    case REASON_ENTITY_TOO_LARGE:
        strncpy(str,reason_code_name[2], str_len);
        break;
    case REASON_OK:
        strncpy(str,reason_code_name[3], str_len);
        break;
    case REASON_INTERNAL_ERROR:
        strncpy(str,reason_code_name[4], str_len);
        break;
    case REASON_NOT_IMPLEMENTED:
        strncpy(str,reason_code_name[5], str_len);
        break;
    }
    return 0;
}
http_general_header_t str_to_http_general_header(const char *gh)
{
    http_general_header_t result = Cache_Control; /* value corresponding to etable[0] */
    for (u_int_t i = 0; general_header[i] != NULL; ++i, ++result)
        if (!(strncasecmp(gh, general_header[i], 16)))
            return result;
    return INVALID_GENERAL_HEADER;
}

http_entity_header_t str_to_http_entity_header(const char *eh)
{
    http_entity_header_t result = Allow; /* value corresponding to etable[0] */
    for (u_int_t i = 0; entity_header[i] != NULL; ++i, ++result)
        if (!(strncasecmp(eh, entity_header[i], 16)))
            return result;
    return INVALID_ENTITY_HEADER;
}
http_response_header_t str_to_http_response_header(const char *resph)
{
    http_response_header_t result = Accept_Ranges; /* value corresponding to etable[0] */
    for (u_int_t i = 0; response_header[i] != NULL; ++i, ++result)
        if (!(strncasecmp(resph, response_header[i], 16)))
            return result;
    return INVALID_RESPONSE_HEADER;
}

http_request_header_t str_to_http_request_header(const char *h)
{

    http_request_header_t result = INVALID_REQUEST_HEADER;
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
                                                        result = Authorization;
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
                            result = Accept;
                            switch (h[6])
                            {
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
                                                            result = Accept_Charset;
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
                                                                result=Accept_Encoding;
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
                                                                result=Accept_Language;
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
                            result=Expect;
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
                    result=From;
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
                    result = Host;
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
                                result = If_Match;
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
                                                                        result = If_Modified_Since;
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
                                                        result = If_None_Match;
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
                                    result = If_Range;
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
                                                                                result = If_Unmodified_Since;
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
                                                    result = Max_Forwards;
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
                                                                                result = Proxy_Authorization;
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
                        result = Range;
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
                                result = Referer;
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
            result = TE;
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
                                            result= User_Agent;
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

    return result;

}

// function must pass args char* str buffer not more than HTTP_HEADER_NAME_MAX_LEN
int http_general_header_to_str(http_general_header_t h, char* str, unsigned char str_len)
{
    if(str_len < HTTP_HEADER_NAME_MAX_LEN)
        return 2;
    switch (h)
    {
    case Cache_Control:
        strncpy(str, general_header[0], str_len);
        break;
    case Connection:
        strncpy(str, general_header[1], str_len);
        break;
    case Date:
        strncpy(str, general_header[2], str_len);
        break;
    case Pragma:
        strncpy(str, general_header[3], str_len);
        break;
    case Trailer:
        strncpy(str, general_header[4], str_len);
        break;
    case Transfer_Encoding:
        strncpy(str, general_header[5], str_len);
        break;
    case Upgrade:
        strncpy(str, general_header[6], str_len);
        break;
    case Via:
        strncpy(str, general_header[7], str_len);
        break;
    case Warning:
        strncpy(str, general_header[8], str_len);
        break;
    default:
        return 1;
    }
    return 0;
}

int http_request_header_to_str(http_request_header_t h, char* str, unsigned char str_len)
{
    if(str_len < HTTP_HEADER_NAME_MAX_LEN)
        return 2;
    switch (h)
    {
    case Accept:
        strncpy(str, request_header[0], str_len);
        break;
    case Accept_Charset:
        strncpy(str, request_header[1], str_len);
        break;
    case Accept_Encoding:
        strncpy(str, request_header[2], str_len);
        break;
    case Accept_Language:
        strncpy(str, request_header[3], str_len);
        break;
    case Authorization:
        strncpy(str, request_header[4], str_len);
        break;
    case Expect:
        strncpy(str, request_header[5], str_len);
        break;
    case From:
        strncpy(str, request_header[6], str_len);
        break;
    case Host:
        strncpy(str, request_header[7], str_len);
        break;
    case If_Match:
        strncpy(str, request_header[8], str_len);
        break;
    case If_Modified_Since:
        strncpy(str, request_header[9], str_len);
        break;
    case If_None_Match:
        strncpy(str, request_header[10], str_len);
        break;
    case If_Range:
        strncpy(str, request_header[11], str_len);
        break;
    case If_Unmodified_Since:
        strncpy(str, request_header[12], str_len);
        break;
    case Max_Forwards:
        strncpy(str, request_header[13], str_len);
        break;
    case Proxy_Authorization:
        strncpy(str, request_header[14], str_len);
        break;
    case Range:
        strncpy(str, request_header[15], str_len);
        break;
    case Referer:
        strncpy(str, request_header[16], str_len);
        break;
    case TE:
        strncpy(str, request_header[17], str_len);
        break;
    case User_Agent:
        strncpy(str, request_header[18], str_len);
        break;
    default:
        return 1;
    }

    return 0;
}

int http_entity_header_to_str(http_entity_header_t h, char* str, unsigned char str_len)
{
    if(str_len < HTTP_HEADER_NAME_MAX_LEN)
        return 2;
    switch (h)
    {
    case Allow:
        strncpy(str, entity_header[0], str_len);
        break;
    case Content_Encoding:
        strncpy(str, entity_header[1], str_len);
        break;
    case Content_Language:
        strncpy(str, entity_header[2], str_len);
        break;
    case Content_Length:
        strncpy(str, entity_header[3], str_len);
        break;
    case Content_Location:
        strncpy(str, entity_header[4], str_len);
        break;
    case Content_MD5:
        strncpy(str, entity_header[5], str_len);
        break;
    case Content_Range:
        strncpy(str, entity_header[6], str_len);
        break;
    case Content_Type:
        strncpy(str, entity_header[7], str_len);
        break;
    case Expires:
        strncpy(str, entity_header[8], str_len);
        break;
    case Last_Modified:
        strncpy(str, entity_header[9], str_len);
        break;
    default:
        return 1;

    }

    return 0;
}
int http_response_header_to_str(http_response_header_t h, char* str, unsigned char str_len)
{
    if(str_len < HTTP_HEADER_NAME_MAX_LEN)
        return 2;
    switch(h)
    {
    case Accept_Ranges:
        strncpy(str, response_header[0], str_len);
        break;
    case  Age:
        strncpy(str, response_header[1], str_len);
        break;
    case   ETag:
        strncpy(str, response_header[2], str_len);
        break;
    case Location:
        strncpy(str, response_header[3], str_len);
        break;
    case Proxy_Authenticate:
        strncpy(str, response_header[4], str_len);
        break;
    case Retry_After:
        strncpy(str, response_header[5], str_len);
        break;
    case Server:
        strncpy(str, response_header[6], str_len);
        break;
    case Vary:
        strncpy(str, response_header[7], str_len);
        break;
    case  WWW_Authenticate:
        strncpy(str, response_header[8], str_len);
        break;
    default:
        return 1;
    }
    return 0;
}
http_header_node_t* init_http_request_header_node(const char http_header_name[], const char http_header_value[])
{
    http_header_node_t* t;
    char http_header[HTTP_HEADER_NAME_MAX_LEN];

    strncpy(http_header,http_header_name, HTTP_HEADER_NAME_MAX_LEN);

    if((t = (http_header_node_t*)malloc(sizeof(http_header_node_t))) == NULL)
        return NULL;
    strncpy(t->http_header_value, http_header_value, HTTP_HEADER_VALUE_MAX_LEN);
    t->next = NULL;

    if((t->http_header=str_to_http_general_header(http_header)) != INVALID_GENERAL_HEADER)
        t->type=http_general_header;
    else if((t->http_header=str_to_http_entity_header(http_header)) != INVALID_ENTITY_HEADER)
        t->type=http_entity_header;
    else if((t->http_header=str_to_http_request_header(http_header)) != INVALID_REQUEST_HEADER)
        t->type=http_request_header;
    else return NULL;

    return t;
}
http_header_node_t* init_http_response_header_node(const char http_header_name[], const char http_header_value[])
{
    http_header_node_t* t;
    char http_header[HTTP_HEADER_NAME_MAX_LEN];

    strncpy(http_header,http_header_name, HTTP_HEADER_NAME_MAX_LEN);

    if((t = (http_header_node_t*)malloc(sizeof(http_header_node_t))) == NULL)
        return NULL;
    strncpy(t->http_header_value, http_header_value, HTTP_HEADER_VALUE_MAX_LEN);
    t->next = NULL;

    if((str_to_http_general_header(http_header)) != INVALID_GENERAL_HEADER)
        t->type=http_general_header;
    else if(str_to_http_entity_header(http_header) != INVALID_ENTITY_HEADER)
        t->type=http_entity_header;
    else if(str_to_http_response_header(http_header) != INVALID_RESPONSE_HEADER)
        t->type=http_request_header;
    else return NULL;

    return t;
}
void destroy_http_header_node(http_header_node_t* header_node)
{   
    free(header_node);

    return;
}

http_headers_list_t* init_http_headers_list(http_header_node_t* first_node)
{
    http_headers_list_t* hl;
    if((hl=(http_headers_list_t*)malloc(sizeof(http_header_node_t))) == NULL)
        return NULL;
    hl->first=first_node;
    hl->limit=64;
    hl->capacity=1;

    return hl;
}
void destroy_http_headers_list(http_headers_list_t* hl)
{
    http_header_node_t* current=hl->first;
    http_header_node_t* tmp;

    while(current->next != NULL)
    {
        tmp=current->next;
        destroy_http_header_node(current);
        current=tmp;
    }
    destroy_http_header_node(current);

    free(hl);
    return;
}

int push_http_header_to_list(http_headers_list_t* list, http_header_node_t* header)
{
    http_header_node_t* current;

    current=list->first;

    while(current->next != NULL)
        current=current->next;

    current->next=header;
    list->capacity++;

    return 0;
}


http_request_t* init_http_request()
{
    http_request_t* hr;

    if((hr=(http_request_t*)malloc(sizeof(http_request_t))) == NULL)
        return NULL;
    hr->headers=NULL;

    return hr;
}
void delete_http_request(http_request_t* hr)
{
    if (hr == NULL)
        return;
    // destroy all objects
    if(hr->headers != NULL)
        destroy_http_headers_list(hr->headers);

    free(hr);
    return;
}

int create_http_request(http_request_t* request, http_headers_list_t* list, http_request_line_t req_line)
{
    request->headers=list;
    request->req_line.method=req_line.method;
    request->req_line.http_version=req_line.http_version;
    strncpy(request->req_line.request_URI, req_line.request_URI, REQUEST_URI_STRING_LENGTH);

    return 0;
}

int create_http_response(http_response_t *response, http_headers_list_t* list, http_status_line_t status)
{

    response->sl=status;
    response->headers=list;

    return 0;
}
int header_name_to_str_value_by_type(const http_header_node_t* node, char name[], char value[] )
{

    switch (node->type)
    {
    case http_entity_header:
        http_entity_header_to_str(node->http_header, name, HTTP_HEADER_NAME_MAX_LEN);// TODO double-check length
        strncpy(value, node->http_header_value, HTTP_HEADER_VALUE_MAX_LEN);
        break;
    case http_general_header:
        http_general_header_to_str(node->http_header, name, HTTP_HEADER_NAME_MAX_LEN);
        strncpy(value, node->http_header_value, HTTP_HEADER_VALUE_MAX_LEN);
        break;
    case http_request_header:
        http_request_header_to_str(node->http_header, name, HTTP_HEADER_NAME_MAX_LEN);
        strncpy(value, node->http_header_value, HTTP_HEADER_VALUE_MAX_LEN);
        break;
    case http_response_header:
        http_response_header_to_str(node->http_header,name, HTTP_HEADER_NAME_MAX_LEN);
        break;
    default:
        return 1;
    }

    return 0;
}
//Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
int status_line_to_str(char* str, http_protocol_version_t p, http_reason_code_t r, unsigned char str_len)
{
    char reason_str[REASON_CODE_NAME_MAX_LENGTH]={0};// text description

    // convert int code into name of reason, ie. 501 -> "501 Not Implemented\0"
    if((reason_code_to_str(reason_str, r, REASON_CODE_NAME_MAX_LENGTH)))
        return -1;

    http_ptorocol_code_to_str(str,p, str_len);// HTTP/1.1
    strncat(str, SP, str_len);// adding space separator
    strncat(str, reason_str, str_len);

    return 0;
}

/* Requests */


int process_http_data(http_request_t *req, char *rd)
{
    if(rd == NULL)
        return -1;
    char *tok;
    u_int_t count=0;
    http_header_node_t* header_node=NULL;
    http_headers_list_t* header_list=NULL;
    http_request_line_t req_line;

    char header[HTTP_HEADER_NAME_MAX_LEN], value[HTTP_HEADER_VALUE_MAX_LEN];

    char *saveptr=NULL;

    //strtok_r is MT-Safe function
    if((tok = strtok_r(rd, CRLF, &saveptr)) == NULL)// search for Request-Line
    {
        req->parsing_result=1;
        return 1;
    }


    if((parse_request_line(&req_line, tok)) != 0)
    {
        req->parsing_result=1;
        return 1;
    }

    while(((tok = strtok_r(NULL, CRLF, &saveptr)) != NULL) && count < HEADERS_LIMIT)
    {
        if((parse_http_header_line(tok, header, value)) != 0)
        {
            req->parsing_result=1;
            return 1;
        }
        if(count == 0)
        {
            if((header_node=init_http_request_header_node(header, value)) == NULL)
                return 1;
            if((header_list=init_http_headers_list(header_node)) == NULL)
                return 1;
        }
        else
        {
            if((header_node=init_http_request_header_node(header, value)) == NULL)
                return 1;
            push_http_header_to_list(header_list, header_node);
        }

        count ++;
    }
    create_http_request(req,header_list,req_line);
    // TODO return a pointer to message_body if request method allows this(i.e.POST)
    return 0;
}
// Function parses Header Line like
// Host: ya.ru
int parse_http_header_line(char* header_line, char* header, char *value)
{
    char *saveptr=NULL;
    char *tok;
    // Header
    if((tok=strtok_r(header_line, "SP:", &saveptr)) == NULL)
        return 1;

    strncpy(header, tok, HTTP_HEADER_NAME_MAX_LEN);
    // Header value
    if((tok=strtok_r(NULL, SP, &saveptr)) == NULL)
        return 1;

    strncpy(value, tok, HTTP_HEADER_VALUE_MAX_LEN);

    return 0;
}
// Function parses Request Line like
// GET /index.php HTTP/1.1
int parse_request_line(http_request_line_t* rl, char* rc)
{
    char *saveptr=NULL;
    char *tok;
    // Request Method
    if((tok=strtok_r(rc, SP, &saveptr)) == NULL)
        return 1;
    if((rl->method=str_to_http_method(tok)) == INVALID_METHOD)
        return INVALID_METHOD;
    // URI
    // According to RFC2616 it may be
    // Request-URI    = "*" | absoluteURI | abs_path | authority
    // * asterisk(allowed in OPTIONS method)
    // absoluteURI like http://www.ya.ru(when request to proxy)
    // abs_path like /index.html
    // authority(for CONNECT method)
    if(( (tok=strtok_r(NULL, SP, &saveptr)) == NULL))
        return 1;

    strncpy(rl->request_URI, tok, REQUEST_URI_STRING_LENGTH);

    // Request Protocol
    if((tok=strtok_r(NULL, "", &saveptr)) == NULL)
        return 1;
    if((rl->http_version=str_to_http_protocol(tok)) == INVALID_PROTO)
        return INVALID_PROTO;

    return 0;
}

int get_current_date_str(char* date)
{

    time_t t = time(NULL);
    struct tm *tm = gmtime(&t);// probably localtime() instead of gmtime()

    strftime(date, 128, "%c", tm);

    return 0;
}

int create_date_header_to_str(char* dheader, unsigned char dheader_len)
{
    //temporary constant date
    char date[128]={0};
    strncpy(dheader, "Date: ", dheader_len);
    //strncpy(dheader, "Date: Fri Dec  7 15:37:41 2018", 48);
    get_current_date_str(date);
    // return 0;
    return strncat(dheader, date, dheader_len) ? 0 : 1;
}

int create_header_name_of_server(char* server_name, size_t len)
{
    strncpy(server_name, "Server: ", 8);
    strncat(server_name, servername, len);
    return 0;
}



int create_error_response(http_response_t* res, const http_reason_code_t code, char *error_file)
{

    return 0;
}



long get_file_size(int fd)
{
    long size, current_pos;

    current_pos =lseek(fd, 0L, SEEK_CUR); //remember current pos
    size=lseek(fd, 0L, SEEK_END); // file size
    lseek(fd,current_pos, SEEK_SET); // return to initial pos

    return size;

}



// converts file extension into mime type
// max file extension length is 5
http_content_type_t get_file_MIME_type(const char* filename)
{
    char *fileExtension;
    if ((fileExtension = strrchr(filename, '.')) == NULL)
        return INVALID_MIME;

    http_content_type_t mime = text_html;

    for (int i = 0; file_ext[i] != NULL; ++i, ++mime)
        if (!(strncmp(fileExtension, file_ext[i], 5)))
            return mime;
    return INVALID_MIME;

}

int file_MIME_type_to_str(char* content_type, const char* filename, unsigned char type_len)
{
    http_content_type_t ct;

    if((ct = get_file_MIME_type(filename)) == INVALID_MIME)
    {
        WriteLog("Invalid MIME type, unsupported file extension");
        return -1;
    }

    if((content_type_to_str(content_type, ct, type_len)) != 0)
    {
        WriteLog("Insufficient buffer length for writing Content Type, content_type=%d", ct);
        return -1;
    }

    return 0;
}

int response_to_str(char* response, const http_response_t* resp)
{

}


int http_method_to_str(http_method_t h, char* str, unsigned char str_len)
{
    if(str_len < HTTP_METHOD_MAX_LEN)
        return 2;
    switch (h)
    {
    case GET:
        strncpy(str, http_method[0], str_len);
        break;
    case POST:
        strncpy(str, http_method[1], str_len);
        break;
    case HEAD:
        strncpy(str, http_method[2], str_len);
        break;
    case PUT:
        strncpy(str, http_method[3], str_len);
        break;
    case DELETE:
        strncpy(str, http_method[4], str_len);
        break;
    case CONNECT:
        strncpy(str, http_method[5], str_len);
        break;
    case OPTIONS:
        strncpy(str, http_method[6], str_len);
        break;
    case TRACE:
        strncpy(str, http_method[7], str_len);
        break;
    case PATCH:
        strncpy(str, http_method[8], str_len);
        break;
    default:
        return 1;

    }

    return 0;
}

