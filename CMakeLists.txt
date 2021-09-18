cmake_minimum_required(VERSION 3.15)

# set the project name
project(Knocker)
set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Source and build directories cannot be the same.")
endif()

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/whoshuu/cpr.git GIT_TAG 1.6.2)
FetchContent_MakeAvailable(cpr)

#libgourou
set(LIBGOUROU_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/libgourou/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libgourou/utils)
set(LIBGOUROU_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libgourou/libgourou.a)
add_custom_target(build_libgourou ALL
    COMMAND ${CMAKE_MAKE_PROGRAM} BUILD_STATIC=1 BUILD_UTILS=0
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libgourou
    COMMENT "Original libgourou makefile target")
add_library(libgourou STATIC IMPORTED)
set_property(TARGET libgourou APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(libgourou PROPERTIES IMPORTED_LOCATION_NOCONFIG "${LIBGOUROU_LIB}")
add_dependencies(libgourou build_libgourou)

#updflibrary
set(UPDFLIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/libgourou/lib/updfparser/libupdfparser.a)
set(UPDFLIBRARY_OBJS
    ${CMAKE_CURRENT_SOURCE_DIR}/libgourou/lib/updfparser/obj/uPDFTypes.o
    ${CMAKE_CURRENT_SOURCE_DIR}/libgourou/lib/updfparser/obj/uPDFParser.o)
add_library(updflibrary STATIC IMPORTED)
set_property(TARGET updflibrary APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(updflibrary PROPERTIES IMPORTED_LOCATION_NOCONFIG "${UPDFLIBRARY}")

set(pugixml_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/libgourou/lib/pugixml/src)


add_subdirectory(lib)

add_subdirectory(src)
