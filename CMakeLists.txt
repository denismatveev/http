cmake_minimum_required(VERSION 3.2)
project(lhttpd)
include_directories(.)
add_library(httpd
http.c 
jobs_queue.c
parse_config.c
worker.c
)
add_executable(lhttpd
main.c
)

add_executable(test_lhttpd
test.c
)
target_link_libraries(lhttpd httpd)
target_link_libraries(test_lhttpd httpd)


