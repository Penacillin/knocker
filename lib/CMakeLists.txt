project(KnockerLibs)
add_library(KnockerLibs knock_drmclient.cpp)

find_library(libzip REQUIRED)
find_package(ZLIB REQUIRED)


set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC
                        OpenSSL::Crypto
                        zip
                        ZLIB::ZLIB
                        updflibrary
                        libgourou
                        cpr::cpr)

target_link_directories(KnockerLibs PUBLIC curlpp)
target_include_directories(KnockerLibs PUBLIC
                          "${curlpp_INCLUDE_DIRS}"
                          "${pugixml_INCLUDE_DIRS}"
                          )
