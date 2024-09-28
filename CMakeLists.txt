cmake_minimum_required(VERSION 3.5)

project(KatanaBrowser)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5/lib/cmake")

# Find Qt5
find_package(Qt5Widgets REQUIRED)

# Find libcurl
find_package(CURL REQUIRED)

add_executable(KatanaBrowser main.cpp)

# Link Qt5 and libcurl
target_link_libraries(KatanaBrowser Qt5::Widgets CURL::libcurl)