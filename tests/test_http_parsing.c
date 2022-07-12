#include "http.h"

void print_request(http_request_t* r)
{

    char proto[HTTP_PROTOCOL_VERSION_MAX_LENGTH],method[16];
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
    http_response_t* response;
    response=init_http_response();// initializing empty response

    int fd;
    char date[DATE_HEADER_MAX_LENGTH];
    get_current_date_str(date);
    request=init_http_request();// dynamically allocated data structure

    char to_be_sent[512]={0};// buffer to be sent


    char raw_string0[]=" GET / HTTP/1.1\r\nHost: ya.ru\r\nUser-Agent: curl\r\n\r\n";//Request-Line should start from alpha symbol. No space or others allowed
    char raw_string1[]="GET / HTTP/1.1\r\nHost: ya.ru\r\n\r\n";
    char raw_string_wrong[]="GETT / HTTP/1.1\r\nHost: ya.ru\r\n\r\n";


    if((process_http_data(request, raw_string0)) == 0)
        print_request(request);
    if((process_http_data(request, raw_string1)) == 0)
        print_request(request);
    if((process_http_data(request, raw_string_wrong)) == 0)
        print_request(request);
    else printf("%s is wrong request\n\n", raw_string_wrong);
    //if we parsed all successfully, set reason code is 200
    set_reason_code(response, 200);
    add_header_to_response(response, "Date:", date);
    add_header_to_response(response, "Server:", "Maya");
    fd=open("index.html", O_RDONLY);
    add_file_as_message_body(response, fd, "index.html");
//    add_header_to_response(response,"Content-Type:", "text/html");
//    add_header_to_response(response,"Content-Length:", "12800");
    //push some additional headers like Content-Type and Content-Length(by name): push_http_header(http_response_t*, char* http_name, char* http_value)
    //Or push by enum(in case of using in web server to prevent double converting(string to number, then number to string): push_http_data(http_response_t*, int header_type, int header_name, char* header_value)
    //TODO process_http_request(http_response_t*, http_request_t*) inside the web server
    process_http_response(to_be_sent,response,512);

    printf("A response to be sent:\n");
    printf("%s\n", to_be_sent);

    delete_http_response(response);
    delete_http_request(request);

    // an example how to use library for creating a response

}
