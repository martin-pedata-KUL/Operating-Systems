cmake_minimum_required(VERSION 4.0)
project(startcodeplab2 C)

set(CMAKE_C_STANDARD 11)

include_directories(ex1)
include_directories(milestone2)

add_executable(startcodeplab2
        ex1/config.h
        ex1/ex2_remade.c
        ex1/main.c
        ex1/sensor_db.c
        ex1/sensor_db.h
        milestone2/config.h
        milestone2/logger.c
        milestone2/logger.h
        milestone2/main.c
        milestone2/sensor_db.c
        milestone2/sensor_db.h)
