cmake_minimum_required(VERSION 3.10)
project(littlesnake C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(littlesnake 
    code/littlesnake.c  
    code/littlesnake.h
    code/main.c
)

# packages under linux
if(UNIX AND NOT APPLE)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
endif()

target_include_directories(littlesnake PRIVATE
    code
)

target_link_libraries(littlesnake 
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_glu_LIBRARY}
    ${GLUT_glut_LIBRARY}
)

# platform-specific settings
if(WIN32)
    target_include_directories(littlesnake PRIVATE
        dependencies/freeglut/include
    )
    target_link_directories(littlesnake PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(littlesnake freeglut freeglutd)
    
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
elseif(UNIX AND NOT APPLE)
    target_link_libraries(littlesnake m)
endif()