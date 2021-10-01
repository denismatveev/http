#include "http.h"

void print_request(http_request_t* r)
{

    char proto[HTTP_PROTOCOL_VERSION_MAX_LENGTH],method[16];
    size_t i;
    http_header_node_t* node;
    char header_name[HTTP_HEADER_NAME_MAX_LEN], header_value[HTTP_HEADER_VALUE_MAX_LEN];
    printf("Request Line:\n");
    http_method_to_str(r->req_line.method,method, 16);
    printf("Method: %s\n",method);
    printf("URI: %s\n",r->req_line.request_URI);
    http_ptorocol_code_to_str(proto, r->req_line.http_version,HTTP_PROTOCOL_VERSION_MAX_LENGTH);
    printf("Protocol: %s\n",proto);
    printf("Headers:\n");

    node=r->headers->first;

    while(node != NULL)
    {
        header_name_to_str_value_by_type(node, header_name, header_value);
        printf("%s %s\n",header_name,header_value);
        node=node->next;
    }
    printf("\n");

}
int main(int argc, char** argv)
{

    // an example how to use library for parsing a request
    http_request_t* request;

    request=init_http_request();// dynamically allocated data structure


    char raw_string0[]=" GET / HTTP/1.1\r\nHost: ya.ru\r\nUser-Agent: curl\r\n\r\n";//Request Line should start from alpha symbol. No space or others allowed
    char raw_string1[]="GET / HTTP/1.1\r\nHost: ya.ru\r\n\r\n";


    process_http_data(request, raw_string0);
    print_request(request);
    process_http_data(request, raw_string1);
    print_request(request);
    //  process_http_data(request, raw_string1);


    delete_http_request(request);

    // an example how to use library for creating a response

}
