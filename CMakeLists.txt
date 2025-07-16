cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(url_downloader)

find_package(Poco REQUIRED COMPONENTS Net SSL Util Foundation)

add_executable(url_downloader
    src/main.cpp
    src/downloader.cpp
)

target_link_libraries(url_downloader
    Poco::Net
    Poco::SSL
    Poco::Util
    Poco::Foundation
)