cmake_minimum_required(VERSION 3.31)
project(Sockets C)

set(CMAKE_C_STANDARD 11)

add_executable(Sockets Client.c)
add_executable(spam tcp_station1.c)
add_executable(server tcp_station2.c)
add_executable(station1_udp udp_station1.c)
add_executable(station2_udp udp_station2.c)
add_executable(multi_th_1_serv multi_th_1_serv.c)
add_executable(multi_th_multiserv multithread_multiservice.c)
add_executable(client_multi client_multiservice.c)