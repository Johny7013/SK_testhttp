cmake_minimum_required(VERSION 3.12)
project(SK_male_zadanie1920 C)

set(CMAKE_C_STANDARD 11)

add_executable(SK_male_zadanie1920 testhttp_raw.c err.c input.c cookies.c utils.c http.c tcp.c)