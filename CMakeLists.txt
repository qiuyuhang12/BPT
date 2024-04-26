cmake_minimum_required(VERSION 3.22)
project(BPT)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
set(CMAKE_CXX_STANDARD 20)
add_executable(code
        submit.cpp
#        STLSrc/exceptions.hpp
#        STLSrc/map.hpp
#        STLSrc/my_print.hpp
#        STLSrc/my_sort.hpp
#        STLSrc/utility.hpp
#        STLSrc/vector.hpp
#        LRU.hpp
#        STLSrc/MapAndVectorForSubmit.hpp
)
#add_executable(multi_test
#        BPT.hpp
#        multi_test.cpp
#        STLSrc/exceptions.hpp
#        STLSrc/map.hpp
#        STLSrc/my_print.hpp
#        STLSrc/my_sort.hpp
#        STLSrc/utility.hpp
#        STLSrc/vector.hpp
#        LRU.hpp
#)
#
#add_executable(test1
#        BPT.hpp
#        test.cpp
#        STLSrc/exceptions.hpp
#        STLSrc/map.hpp
#        STLSrc/my_print.hpp
#        STLSrc/my_sort.hpp
#        STLSrc/utility.hpp
#        STLSrc/vector.hpp
#        LRU.hpp
#)
#add_executable(test2
#        BPT.hpp
#        test2.cpp
#        STLSrc/exceptions.hpp
#        STLSrc/map.hpp
#        STLSrc/my_print.hpp
#        STLSrc/my_sort.hpp
#        STLSrc/utility.hpp
#        STLSrc/vector.hpp
#        LRU.hpp
#)
#add_executable(MemoryRiverPhaser
#        MemoryRiverPhaser.cpp
#        STLSrc/exceptions.hpp
#        STLSrc/map.hpp
#        STLSrc/my_print.hpp
#        STLSrc/my_sort.hpp
#        STLSrc/utility.hpp
#        STLSrc/vector.hpp
#        LRU.hpp
#)
#add_executable(LRUTest
#        LRUTest.cpp
##        STLSrc/exceptions.hpp
##        STLSrc/map.hpp
##        STLSrc/my_print.hpp
##        STLSrc/my_sort.hpp
##        STLSrc/utility.hpp
##        STLSrc/vector.hpp
#        LRU.hpp
#        LRUTest.cpp
#)
#add_executable(code
#        MemoryRiverPhaser.cpp
#        STLSrc/exceptions.hpp
#        STLSrc/map.hpp
#        STLSrc/my_print.hpp
#        STLSrc/my_sort.hpp
#        STLSrc/utility.hpp
#        STLSrc/vector.hpp
#        LRU.hpp
#)
