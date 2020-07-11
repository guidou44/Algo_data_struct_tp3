
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(BINARY ${CMAKE_PROJECT_NAME})

set(SOURCE_FILES
    Dictionnaire.cpp
    Dictionnaire.h
    Principal.cpp)

add_executable(TP3 ${SOURCE_FILES})
add_library(${BINARY}_lib STATIC ${SOURCE_FILES})