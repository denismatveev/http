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

static char *entity_header_content_type_str[] =
{
    #define XX(num, name, entity_content_type_str) #entity_content_type_str,
    HTTP_ENTITY_HEADER_CONTENT_TYPE(XX)
    #undef XX
};
static char *content_type_str[] =
{
    #define XX(num, name, content_type_str) #content_type_str,
    HTTP_CONTENT_TYPE(XX)
    #undef XX
};
static char *general_header_str[] =
{
    #define XX(num, name, general_header_str) #general_header_str,
    HTTP_GENERAL_HEADER(XX)
    #undef XX
};
static char *file_ext[] = {".html", ".jpg", ".pdf", ".png", ".mpeg", ".css", NULL};

static char* response_header_str[] =
{
    #define XX(num, name, response_header_str) #response_header_str,
    HTTP_RESPONSE_HEADER(XX)
    #undef XX
};

// the following is used in both request and response

static char* entity_header_str[] =
{
    #define XX(num, name, entity_header_str) #entity_header_str,
    HTTP_ENTITY_HEADER(XX)
    #undef XX
};

static char* request_header_str[] =
{
    #define XX(num, name, request_header_str) #request_header_str,
    HTTP_REQUEST_HEADER(XX)
    #undef XX
};
static char* http_method_str[] =
{
    #define XX(num, name, http_method_str) #http_method_str,
    HTTP_METHOD(XX)
    #undef XX
};

// errors

static char* bad_request_error = "<html><body><head><title>400 Bad Request</title></head><center><h1> 400 Bad Request <h1></center></body></html>";
static char* not_found_error = "<html><body><head><title>404 Not Found</title></head><center><h1> 404 Not Found <h1></center></body></html>";
static char* internal_error = "<html><body><head><title>500 Internal Error</title></head><center><h1> 500 Internal Error <h1></center></body></html>";
static char* not_implemented_error = "<html><body><head><title>501 Not Implemented</title></head><center><h1> 501 Not Implemented <h1></center></body></html>";

// According to RFC2616 a method must be case sensitive
// Implied the passing string is null terminated, but doesn't verified if there is '\0' at the end position
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

                switch (sval[3])
                {// checking if there is any else symbol
                case '!' ... '~'://gcc extension only
                    return INVALID_METHOD;
                }
                result = HTTP_METHOD_GET;
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
                    result = HTTP_METHOD_POST;
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
                result = HTTP_METHOD_PUT;
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
                        result = HTTP_METHOD_PATCH;
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
                    switch (sval[4])
                    {
                    case '!' ... '~':
                        return INVALID_METHOD;
                    }
                    result = HTTP_METHOD_HEAD;
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
                            result = HTTP_METHOD_DELETE;
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
                                result = HTTP_METHOD_CONNECT;
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
                                result = HTTP_METHOD_OPTIONS;
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
                        result = HTTP_METHOD_TRACE;
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

int content_type_to_str(char *str, http_entity_header_content_type_t ct, unsigned char str_len)
{
    if(str_len < CONTENT_TYPE_MAX_LENGTH)
        return 2;
    switch (ct)
    {
    case CONTENT_TEXT_HTML:
        strncpy(str,entity_header_content_type_str[0], str_len);
        break;
    case CONTENT_IMAGE_JPG:
        strncpy(str,entity_header_content_type_str[1], str_len);
        break;
    case CONTENT_APPLICATION_PDF:
        strncpy(str,entity_header_content_type_str[2], str_len);
        break;
    case CONTENT_IMAGE_PNG:
        strncpy(str,entity_header_content_type_str[3], str_len);
        break;
    case CONTENT_VIDEO_MPEG:
        strncpy(str,entity_header_content_type_str[4], str_len);
        break;
    case CONTENT_TEXT_CSS:
        strncpy(str,entity_header_content_type_str[5], str_len);
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
    case REASON_OK:
        strncpy(str,reason_code_name[0], str_len);
        break;
    case REASON_BAD_REQUEST:
        strncpy(str, reason_code_name[0], str_len);
        break;
    case REASON_NOT_FOUND:
        strncpy(str,reason_code_name[1], str_len);
        break;
    case REASON_ENTITY_TOO_LARGE:
        strncpy(str,reason_code_name[2], str_len);
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
    http_general_header_t result = GENERAL_HEADER_CACHE_CONTROL;
    for (u_int_t i = 0; general_header_str[i] != NULL; ++i, ++result)
        if (!(strncasecmp(gh, general_header_str[i], 16)))
            return result;
    return INVALID_GENERAL_HEADER;

}

http_entity_header_t str_to_http_entity_header(const char *eh)
{
    http_entity_header_t result = ENTITY_HEADER_ALLOW; /* value corresponding to etable[0] */
    for (u_int_t i = 0; entity_header_str[i] != NULL; ++i, ++result)
        if (!(strncasecmp(eh, entity_header_str[i], 16)))
            return result;
    return INVALID_ENTITY_HEADER;
}
http_response_header_t str_to_http_response_header(const char *resph)
{
    http_response_header_t result = RESPONSE_HEADER_ACCEPT_RANGES; /* value corresponding to etable[0] */
    for (u_int_t i = 0; response_header_str[i] != NULL; ++i, ++result)
        if (!(strncasecmp(resph, response_header_str[i], 16)))
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
                                                        result = REQUEST_HEADER_AUTHORIZATION;
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            }
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
                            result = REQUEST_HEADER_ACCEPT;
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
                                                            result = REQUEST_HEADER_ACCEPT_CHARSET;
                                                            break;
                                                        }
                                                        break;
                                                    }
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
                                                                result = REQUEST_HEADER_ACCEPT_ENCODING;
                                                                break;
                                                            }
                                                            break;
                                                        }
                                                        break;

                                                    }
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
                                                                result = REQUEST_HEADER_ACCEPT_LANGUAGE;
                                                                break;
                                                            }
                                                            break;
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            }
                            break;
                        }
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
                            result = REQUEST_HEADER_EXPECT;
                            break;
                        }
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
                    result = REQUEST_HEADER_FROM;
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
                    result = REQUEST_HEADER_HOST;
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
                                result = REQUEST_HEADER_IF_MATCH;
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
                                                                        result = REQUEST_HEADER_IF_MODIFIED_SINCE;
                                                                        break;
                                                                    }
                                                                    break;
                                                                }
                                                                break;
                                                            }
                                                            break;
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
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
                                                        result = REQUEST_HEADER_IF_NONE_MATCH;
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
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
                                    result = REQUEST_HEADER_IF_RANGE;
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
                                                                                result = REQUEST_HEADER_IF_UNMODIFIED_SINCE;
                                                                                break;
                                                                            }
                                                                            break;
                                                                        }
                                                                        break;
                                                                    }
                                                                    break;
                                                                }
                                                                break;
                                                            }
                                                            break;
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            }
                            break;
                        }
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
                                                    result = REQUEST_HEADER_MAX_FORWARDS;
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            }
                            break;
                        }
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
                                                                                result = REQUEST_HEADER_PROXY_AUTHORIZATION;
                                                                                break;
                                                                            }
                                                                            break;
                                                                        }
                                                                        break;
                                                                    }
                                                                    break;
                                                                }
                                                                break;
                                                            }
                                                            break;
                                                        }
                                                        break;
                                                    }
                                                    break;
                                                }
                                                break;
                                            }
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            }
                            break;
                        }
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
                        result = REQUEST_HEADER_RANGE;
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
                                result = REQUEST_HEADER_REFERER;
                                break;
                            }
                            break;
                        }
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
            result = REQUEST_HEADER_TE;
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
                                            result= REQUEST_HEADER_USER_AGENT;
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                                break;
                            }
                            break;
                        }
                        break;
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
    case GENERAL_HEADER_CACHE_CONTROL:
        strncpy(str, general_header_str[0], str_len);
        break;
    case GENERAL_HEADER_CONNECTION:
        strncpy(str, general_header_str[1], str_len);
        break;
    case GENERAL_HEADER_DATE:
        strncpy(str, general_header_str[2], str_len);
        break;
    case GENERAL_HEADER_PRAGMA:
        strncpy(str, general_header_str[3], str_len);
        break;
    case GENERAL_HEADER_TRAILER:
        strncpy(str, general_header_str[4], str_len);
        break;
    case GENERAL_HEADER_TRANSFER_ENCODING:
        strncpy(str, general_header_str[5], str_len);
        break;
    case GENERAL_HEADER_UPGRADE:
        strncpy(str, general_header_str[6], str_len);
        break;
    case GENERAL_HEADER_VIA:
        strncpy(str, general_header_str[7], str_len);
        break;
    case GENERAL_HEADER_WARNING:
        strncpy(str, general_header_str[8], str_len);
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
    case REQUEST_HEADER_ACCEPT:
        strncpy(str, request_header_str[0], str_len);
        break;
    case REQUEST_HEADER_ACCEPT_CHARSET:
        strncpy(str, request_header_str[1], str_len);
        break;
    case REQUEST_HEADER_ACCEPT_ENCODING:
        strncpy(str, request_header_str[2], str_len);
        break;
    case REQUEST_HEADER_ACCEPT_LANGUAGE:
        strncpy(str, request_header_str[3], str_len);
        break;
    case REQUEST_HEADER_AUTHORIZATION:
        strncpy(str, request_header_str[4], str_len);
        break;
    case REQUEST_HEADER_EXPECT:
        strncpy(str, request_header_str[5], str_len);
        break;
    case REQUEST_HEADER_FROM:
        strncpy(str, request_header_str[6], str_len);
        break;
    case REQUEST_HEADER_HOST:
        strncpy(str, request_header_str[7], str_len);
        break;
    case REQUEST_HEADER_IF_MATCH:
        strncpy(str, request_header_str[8], str_len);
        break;
    case REQUEST_HEADER_IF_MODIFIED_SINCE:
        strncpy(str, request_header_str[9], str_len);
        break;
    case REQUEST_HEADER_IF_NONE_MATCH:
        strncpy(str, request_header_str[10], str_len);
        break;
    case REQUEST_HEADER_IF_RANGE:
        strncpy(str, request_header_str[11], str_len);
        break;
    case REQUEST_HEADER_IF_UNMODIFIED_SINCE:
        strncpy(str, request_header_str[12], str_len);
        break;
    case REQUEST_HEADER_MAX_FORWARDS:
        strncpy(str, request_header_str[13], str_len);
        break;
    case REQUEST_HEADER_PROXY_AUTHORIZATION:
        strncpy(str, request_header_str[14], str_len);
        break;
    case REQUEST_HEADER_RANGE:
        strncpy(str, request_header_str[15], str_len);
        break;
    case REQUEST_HEADER_REFERER:
        strncpy(str, request_header_str[16], str_len);
        break;
    case REQUEST_HEADER_TE:
        strncpy(str, request_header_str[17], str_len);
        break;
    case REQUEST_HEADER_USER_AGENT:
        strncpy(str, request_header_str[18], str_len);
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
    case ENTITY_HEADER_ALLOW:
        strncpy(str, entity_header_str[0], str_len);
        break;
    case ENTITY_HEADER_CONTENT_ENCODING:
        strncpy(str, entity_header_str[1], str_len);
        break;
    case ENTITY_HEADER_CONTENT_LANGUAGE:
        strncpy(str, entity_header_str[2], str_len);
        break;
    case ENTITY_HEADER_CONTENT_LENGTH:
        strncpy(str, entity_header_str[3], str_len);
        break;
    case ENTITY_HEADER_CONTENT_LOCATION:
        strncpy(str, entity_header_str[4], str_len);
        break;
    case ENTITY_HEADER_CONTENT_MD5:
        strncpy(str, entity_header_str[5], str_len);
        break;
    case ENTITY_HEADER_CONTENT_RANGE:
        strncpy(str, entity_header_str[6], str_len);
        break;
    case ENTITY_HEADER_CONTENT_TYPE:
        strncpy(str, entity_header_str[7], str_len);
        break;
    case ENTITY_HEADER_EXPIRES:
        strncpy(str, entity_header_str[8], str_len);
        break;
    case ENTITY_HEADER_LAST_MODIFIED:
        strncpy(str, entity_header_str[9], str_len);
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
    case RESPONSE_HEADER_ACCEPT_RANGES:
        strncpy(str, response_header_str[0], str_len);
        break;
    case  RESPONSE_HEADER_AGE:
        strncpy(str, response_header_str[1], str_len);
        break;
    case   RESPONSE_HEADER_ETAG:
        strncpy(str, response_header_str[2], str_len);
        break;
    case RESPONSE_HEADER_LOCATION:
        strncpy(str, response_header_str[3], str_len);
        break;
    case RESPONSE_HEADER_PROXY_AUTHENTICATE:
        strncpy(str, response_header_str[4], str_len);
        break;
    case RESPONSE_HEADER_RETRY_AFTER:
        strncpy(str, response_header_str[5], str_len);
        break;
    case RESPONSE_HEADER_SERVER:
        strncpy(str, response_header_str[6], str_len);
        break;
    case RESPONSE_HEADER_VARY:
        strncpy(str, response_header_str[7], str_len);
        break;
    case  RESPONSE_HEADER_WWW_AUTHENTICATE:
        strncpy(str, response_header_str[8], str_len);
        break;
    default:
        return 1;
    }
    return 0;
}
http_header_node_t* init_http_request_header_node(const char http_header_name[], const char http_header_value[])
{
    http_header_node_t* t;
    http_header_type_t type;
    int http_header;



    if((http_header=str_to_http_general_header(http_header_name)) != INVALID_GENERAL_HEADER)
        type=http_general_header;
    else if((http_header=str_to_http_entity_header(http_header_name)) != INVALID_ENTITY_HEADER)
        type=http_entity_header;
    else if((http_header=str_to_http_request_header(http_header_name)) != INVALID_REQUEST_HEADER)
        type=http_request_header;
    else
        return NULL;

    if((t = (http_header_node_t*)malloc(sizeof(http_header_node_t))) == NULL)
        return NULL;
    t->type=type;
    t->http_header=http_header;
    strncpy(t->http_header_value, http_header_value, HTTP_HEADER_VALUE_MAX_LEN);
    t->next = NULL;

    return t;
}
http_header_node_t* init_http_response_header_node(const char http_header_name[], const char http_header_value[])
{
    http_header_node_t* t;
    http_header_type_t type;
    int http_header;

    if((http_header=str_to_http_general_header(http_header_name)) != INVALID_GENERAL_HEADER)
        type=http_general_header;
    else if((http_header=str_to_http_entity_header(http_header_name)) != INVALID_ENTITY_HEADER)
        type=http_entity_header;
    else if((http_header=str_to_http_response_header(http_header_name)) != INVALID_RESPONSE_HEADER)
        type=http_response_header;
    else
        return NULL;

    if((t = (http_header_node_t*)malloc(sizeof(http_header_node_t))) == NULL)
        return NULL;

    if(http_header == ENTITY_HEADER_CONTENT_TYPE )
        if((validation_content_type(http_header_value)) != 0)
            return NULL;
    t->type=type;
    t->http_header=http_header;
    strncpy(t->http_header_value, http_header_value, HTTP_HEADER_VALUE_MAX_LEN);
    t->next = NULL;

    return t;
}
int validation_content_type(const char *ct)
{

    char buff[HTTP_HEADER_VALUE_MAX_LEN];
    memset(buff, 0, HTTP_HEADER_VALUE_MAX_LEN);
    strncat(buff, entity_header_str[ENTITY_HEADER_CONTENT_TYPE], HTTP_HEADER_VALUE_MAX_LEN);
    strncat(buff," ", 64);
    strncat(buff,ct, 64);
    for (int i = 0; entity_header_content_type_str[i] != NULL; ++i)
        if (!(strncmp(buff, entity_header_content_type_str[i], HTTP_HEADER_VALUE_MAX_LEN)))
            return 0;
    return INVALID_ENTITY_HEADER_MIME;
}
void destroy_http_header_node(http_header_node_t* header_node)
{
    if(header_node == NULL)
        return;
    free(header_node);

    return;
}

http_headers_list_t* init_http_headers_list(http_header_node_t* first_node)
{
    http_headers_list_t* hl;
    if(first_node == NULL)
        return NULL;
    if((hl=(http_headers_list_t*)malloc(sizeof(http_header_node_t))) == NULL)
        return NULL;
    hl->first=first_node;
    hl->limit=64;
    hl->capacity=1;

    return hl;
}
void destroy_http_headers_list(http_headers_list_t* hl)
{
    http_header_node_t* current;
    http_header_node_t* tmp;
    if(hl == NULL)
        return;

    current=hl->first;

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
    if(list == NULL || header == NULL)
        return -1;

    if(list->capacity < list->limit)
    {
        current=list->first;

        while(current->next != NULL)
            current=current->next;

        current->next=header;
        list->capacity++;
        return 0;
    }

    return 1;


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
        http_entity_header_to_str(node->http_header, name, HTTP_HEADER_NAME_MAX_LEN);// TODO double-check the length
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
        strncpy(value, node->http_header_value, HTTP_HEADER_VALUE_MAX_LEN);
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

    http_ptorocol_code_to_str(str, p, str_len);// HTTP/1.1
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

http_response_t* init_http_response(void)
{
    http_response_t* response;

    if((response = (http_response_t*)malloc(sizeof(http_response_t)))== NULL)
        return NULL;

    response->headers=NULL;
    response->sl.rc=0;
    response->sl.hv=PROTO_HTTP11;// server implemented to serve HTTP/1.1
    memset(response->message_body, 0, sizeof(response->message_body));

    return response;
}
void delete_http_response(http_response_t* response)
{
    if(response == NULL)
        return;
    if(response->headers != NULL)
        destroy_http_headers_list(response->headers);
    free(response);
}

int get_current_date_str(char* date)
{

    setlocale(LC_TIME, "POSIX");
    time_t t = time(NULL);
    struct tm *tm = gmtime(&t);

    strftime(date, DATE_HEADER_MAX_LENGTH, "%a, %d %b %Y %T %Z", tm);

    return 0;
}

// TODO replace by stat() ?
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
http_entity_header_content_type_t get_file_MIME_type(const char* filename)
{
    char *fileExtension;
    if ((fileExtension = strrchr(filename, '.')) == NULL)
        return INVALID_ENTITY_HEADER_MIME;

    http_entity_header_content_type_t mime = CONTENT_TEXT_HTML;

    for (int i = 0; file_ext[i] != NULL; ++i, ++mime)
        if (!(strncmp(fileExtension, file_ext[i], 5)))
            return mime;
    return INVALID_ENTITY_HEADER_MIME;

}

int file_MIME_type_to_str(char* content_type, const char* filename, unsigned char type_len)
{
    http_entity_header_content_type_t ct;

    if((ct = get_file_MIME_type(filename)) == INVALID_ENTITY_HEADER_MIME)
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

// creates a serialized response to be sent to client
int process_http_response(char* response, http_response_t* rs, size_t str_len)
{
    char header_name[HEADER_VALUE_MAX_LENGTH],header_value[HEADER_VALUE_MAX_LENGTH];
    char status_line[STATUS_LINE_MAX_LENGTH];
    http_header_node_t *cur;

    if(rs->sl.hv == 0 || rs->sl.rc == 0)
        return -1;
    status_line_to_str(status_line, rs->sl.hv, rs->sl.rc, STATUS_LINE_MAX_LENGTH);


    strncat(response, status_line, str_len);
    strncat(response, CRLF,str_len);
    cur=rs->headers->first;
    // pushing all headers
    while(cur != NULL)
    {
        header_name_to_str_value_by_type(cur, header_name, header_value);
        strncat(response, header_name,str_len);
        strncat(response, SP ,str_len);
        strncat(response, header_value,str_len);
        strncat(response,CRLF,str_len);

        cur=cur->next;
    }
    strncat(response,CRLF,str_len);

    return 0;
}

int set_reason_code(http_response_t* r, int code)
{

    switch (code)
    {
    case REASON_OK:
        break;
    case REASON_BAD_REQUEST:
        break;
    case REASON_ENTITY_TOO_LARGE:
        break;
    case REASON_INTERNAL_ERROR:
        break;
    case REASON_NOT_IMPLEMENTED:
        break;
    case REASON_NOT_FOUND:
        break;
    default:
        return -1;
    }

    //TODO check if code is in emun
    r->sl.rc=(http_reason_code_t)code;
    return 0;

}

int add_header_to_response(http_response_t* resp, char* header_name, char* header_value)
{
    http_headers_list_t* list=NULL;
    http_header_node_t *node;
    int ret;

    if( header_name != NULL && header_value != NULL)
        if((node=init_http_response_header_node(header_name, header_value)) == NULL)
            return -1;

    if(resp->headers == NULL)
    {
        if((list=init_http_headers_list(node)) == NULL)
        {
            destroy_http_header_node(node);
            return -1;
        }
        resp->headers=list;
        return 0;
    }

    if((ret=push_http_header_to_list(resp->headers, node)) == -1)
        return -1;
    else if(ret == 1)
    {
        WriteLog("Too many headers in response");
        return 1;
    }


    return 0;
}

int http_method_to_str(http_method_t h, char* str, unsigned char str_len)
{
    if(str_len < HTTP_METHOD_MAX_LEN)
        return 2;
    switch (h)
    {
    case HTTP_METHOD_GET:
        strncpy(str, http_method_str[0], str_len);
        break;
    case HTTP_METHOD_POST:
        strncpy(str, http_method_str[1], str_len);
        break;
    case HTTP_METHOD_HEAD:
        strncpy(str, http_method_str[2], str_len);
        break;
    case HTTP_METHOD_PUT:
        strncpy(str, http_method_str[3], str_len);
        break;
    case HTTP_METHOD_DELETE:
        strncpy(str, http_method_str[4], str_len);
        break;
    case HTTP_METHOD_CONNECT:
        strncpy(str, http_method_str[5], str_len);
        break;
    case HTTP_METHOD_OPTIONS:
        strncpy(str, http_method_str[6], str_len);
        break;
    case HTTP_METHOD_TRACE:
        strncpy(str, http_method_str[7], str_len);
        break;
    case HTTP_METHOD_PATCH:
        strncpy(str, http_method_str[8], str_len);
        break;
    default:
        return 1;

    }

    return 0;
}
int add_file_as_message_body(http_response_t * response, int fd, const char* filename)
{
    // this function adds only entity headers allowing sending files in a response
    long size;
    char content_type[128];
    http_entity_header_content_type_t ct;

    if(response == NULL || fd == 0)
        return -1;

    size=get_file_size(fd);


    //    file_MIME_type_to_str(content_type, filename, 64);
    if((ct = get_file_MIME_type(filename)) == INVALID_ENTITY_HEADER_MIME)
    {
        WriteLog("Invalid MIME type, unsupported file extension");
        return -1;
    }
    if((add_header_to_response(response, entity_header_str[ENTITY_HEADER_CONTENT_TYPE], content_type_str[ct])) != 0)
        return 1;
    long_to_str(content_type, size);
    if((add_header_to_response(response, entity_header_str[ENTITY_HEADER_CONTENT_LENGTH], content_type)) !=0 )
        return 1;

    // then use sendfile() in a caller to send data to a client

    return 0;
}


// this function is useful to read response from cgi, fastcgi server or similar
int add_message_body(http_response_t * response, int fd)
{
    long size=0;
    char content_type_len[128];
    if(response == NULL || fd == 0)
        return -1;

    //TODO implement errors handle
    size = read(fd, response->message_body, MAX_BODY_SIZE);// TODO timeout of reading from socket

    if((add_header_to_response(response, entity_header_str[ENTITY_HEADER_CONTENT_TYPE], content_type_str[CONTENT_TEXT_HTML])) !=0)
        return 1;
    long_to_str(content_type_len, size);
    if((add_header_to_response(response, entity_header_str[ENTITY_HEADER_CONTENT_LENGTH], content_type_len)) != 0)
        return 1;

    return 0;
}
