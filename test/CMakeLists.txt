set(BINARY ${CMAKE_PROJECT_NAME}_test)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(SOURCE_FILES main.cpp Dictionnaire-test.cpp)
add_executable(${BINARY} ${SOURCE_FILES})
add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib gtest)