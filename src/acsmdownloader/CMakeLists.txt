set(MODULE_NAME acsmdownloader)

add_executable(${MODULE_NAME} main.cc)
set_target_properties(${MODULE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_include_directories(${MODULE_NAME} PUBLIC
                        "${LIBGOUROU_INCLUDE}")

target_link_libraries(${MODULE_NAME}
    KnockerLibs
    cpr::cpr
)