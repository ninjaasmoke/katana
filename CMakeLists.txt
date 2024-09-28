cmake_minimum_required(VERSION 3.5)

project(KatanaBrowser)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5/lib/cmake")

# Find Qt5 components
find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
find_package(Qt5 COMPONENTS Widgets WebEngineWidgets REQUIRED)

# Find libcurl
find_package(CURL REQUIRED)

add_executable(KatanaBrowser main.cpp)

# Link Qt5 and libcurl
target_link_libraries(KatanaBrowser Qt5::Widgets Qt5::WebEngineWidgets CURL::libcurl)

# Set target properties
set_target_properties(KatanaBrowser PROPERTIES 
    MACOSX_BUNDLE TRUE 
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.katana.browser"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Enable CPack for packaging
include(CPack)

set(CPACK_GENERATOR "DMG") 
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CPACK_PACKAGE_NAME "KatanaBrowser")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION "A boring web browser")
set(CPACK_PACKAGE_VENDOR "Katana")
set(CPACK_PACKAGE_CONTACT "nithins674@gmail.com")
set(CPACK_PACKAGE_LICENSE "MIT")

# Specify additional files or directories to include in the package
# set(CPACK_RESOURCE_FILE_ICONS "${CMAKE_CURRENT_SOURCE_DIR}/path/to/icon.icns")
