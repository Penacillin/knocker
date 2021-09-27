set(MODULE_NAME KnockerLibs)

add_library(${MODULE_NAME} STATIC knock_drmclient.cpp)

FIND_PATH(LIBZIP_INCLUDE_DIR zip.h
    $ENV{LIBZIP_DIR}/include
    $ENV{LIBZIP_DIR}
    /usr/local/include
    /usr/include
)

FIND_LIBRARY(LIBZIP_LIBRARY
    NAMES libzip zip
    PATHS
    $ENV{LIBZIP_DIR}/lib
    $ENV{LIBZIP_DIR}
    /usr/local/lib
    /usr/lib
    REQUIRED
)
find_package(ZLIB REQUIRED)

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

target_include_directories(${MODULE_NAME} PUBLIC
    "${pugixml_INCLUDE_DIRS}"
    "${LIBGOUROU_INCLUDE}"
)

target_link_libraries(${MODULE_NAME} PUBLIC
                        OpenSSL::Crypto
                        zip
                        ZLIB::ZLIB
                        ${UPDFLIBRARY_OBJS}
                        libgourou
                        cpr::cpr)
# target_link_libraries(${MODULE_NAME} PUBLIC ${LIBGOUROU_LIB})