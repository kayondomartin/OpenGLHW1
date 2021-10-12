cmake_minimum_required(VERSION 3.20)
project(OpenGLHW1)

set(CMAKE_CXX_STANDARD 14)

add_executable(OpenGLHW1 main.cpp)
target_link_libraries(OpenGLHW1 -lglut -lGL -lGLU)
