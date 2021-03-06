cmake_minimum_required(VERSION 3.2)
#swd - simple web daemon
project(swd)
include_directories(.)
#add_definitions(-DDEBUG -DQUEUEDEBUG)
add_library(http
    http.c
    common.c
    )
add_library(swdlib
    worker.c
    jobs_queue.c
    parse_config.c
    http.c
    common.c
    associative.c
    config.c
    queue.c
    )
add_library(parse_config
    parse_config.c
    common.c
    )
add_library(RBTree
    RBTree.c)
add_library(queues
    queue.c)
add_executable(swd
    main.c
    )

add_executable(test_swd_queues
    tests/jobsqueue_test.c
    )

add_executable(test_assoc
    tests/associative_test.c
    )
add_executable(test_queues
    queue.c
    common.c
    config.c
    associative.c
    RBTree.c
    tests/queues_test.c)
add_executable(test_rbtree
    tests/rbtree_test.c
    config.c
    queue.c
    associative.c
    common.c)
target_link_libraries(swd swdlib pthread)
target_link_libraries(test_swd_queues http swdlib pthread)
target_link_libraries(test_assoc swdlib)
target_link_libraries(test_rbtree RBTree)
