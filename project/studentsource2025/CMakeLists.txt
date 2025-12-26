cmake_minimum_required(VERSION 3.16)
project(sensor_gateway C)

set(CMAKE_C_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(sensor_gateway
        main.c
        connmgr.c
        datamgr.c
        storagemgr.c
        sensor_db.c
        sbuffer.c
        lib/tcpsock.c
        sensor_node.c
        file_creator.c
        lib/dplist.c
)
