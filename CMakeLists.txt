cmake_minimum_required(VERSION 3.10)

# set the project name
project(Knocker)

add_subdirectory(curlpp)

# add the executable
add_executable(Knocker src/main.cc)

include_directories(libgourou/include)
include_directories(libgourou/utils)
target_include_directories(Knocker PUBLIC
                        "${PROJECT_BINARY_DIR}"
                        "${PROJECT_SOURCE_DIR}/curlpp")


#libgourou
set(LIBGOUROU_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libgourou/libgourou.a)
add_custom_target(build_libgourou ALL
COMMAND ${CMAKE_MAKE_PROGRAM} BUILD_STATIC=1 BUILD_UTILS=0
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libgourou
COMMENT "Original libgourou makefile target")
add_library(libgourou STATIC IMPORTED)
set_property(TARGET libgourou APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(libgourou PROPERTIES
IMPORTED_LOCATION_NOCONFIG "${LIBGOUROU_LIB}")
add_dependencies(libgourou build_libgourou)

#updflibrary
file(GLOB UPDFLIBRARY_OBJS "${CMAKE_CURRENT_SOURCE_DIR}/libgourou/lib/updfparser/obj/*")
set(UPDFLIBRARY ${UPDFLIBRARY_OBJS})
add_library(updflibrary STATIC IMPORTED)
set_property(TARGET updflibrary APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(updflibrary PROPERTIES
IMPORTED_LOCATION_NOCONFIG "${UPDFLIBRARY}")


set(curlpp_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/curlpp/include)
set(pugixml_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/libgourou/lib/pugixml/src)
add_subdirectory(lib)
#curlpp
target_link_directories(Knocker PUBLIC curlpp libgourou)
target_link_libraries(Knocker PRIVATE KnockerLibs )
target_link_libraries(Knocker PRIVATE ${UPDFLIBRARY_OBJS})