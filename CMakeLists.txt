cmake_minimum_required(VERSION 3.10)

# set the project name
project(Knocker)

add_subdirectory(curlpp)
add_library(libcurlpp curlpp)

# add the executable
add_executable(Knocker src/main.cc)

target_link_directories(Knocker PUBLIC curlpp)

target_include_directories(Knocker PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/MathFunctions"
                          )