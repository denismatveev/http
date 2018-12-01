cmake_minimum_required(VERSION 3.2)
#swd - simple web daemon
project(swd)
include_directories(.)
add_library(http
    http.c
    common.c
    )
add_library(swdlib
    listener.c
    worker.c
    jobs_queue.c
    parse_config.c
    http.c
    common.c
    )

add_executable(swd
    main.c
    )

#add_executable(test_swd
#    #    test.c
#    http_test.c
#    )
target_link_libraries(swd swdlib pthread)
#target_link_libraries(test_swd http swdlib pthread)

