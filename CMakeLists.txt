cmake_minimum_required(VERSION 3.29)
project(Cache_Sim)

set(CMAKE_CXX_STANDARD 20)

add_executable(Cache_Sim sim.c
        src/Cache.c
        src/CacheOperation.c
        include/Cache.h
        include/CacheOperation.h
        include/LinkedList.h
        include/Prefetch.h
        include/Queue.h
        include/sim.h
        src/LinkedList.c
        src/Prefetch.c
        src/Queue.c)
