cmake_minimum_required(VERSION 3.10)
project(littlesnake C)

set(CMAKE_C_COMPILER clang)
set(CMAKE_C_STANDARD 99)

add_executable(littlesnake 
    code/littlesnake.c  code/littlesnake.h
    code/main.c
)

target_include_directories(littlesnake PRIVATE
    code
    dependencies/freeglut/include
)

target_link_directories(littlesnake PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(littlesnake freeglut freeglutd)

# Only copy DLLs for runtime (POST_BUILD is fine for this)
if(WIN32)
    add_custom_command(TARGET littlesnake POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/freeglut.dll
            ${CMAKE_CURRENT_BINARY_DIR}/
    )
    add_custom_command(TARGET littlesnake POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/freeglutd.dll
            ${CMAKE_CURRENT_BINARY_DIR}/
    )
endif()