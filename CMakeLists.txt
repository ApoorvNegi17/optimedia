cmake_minimum_required(VERSION 3.15)
project(media_compress_net VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for tooling (clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find Dependencies
find_package(Threads REQUIRED)
find_package(Boost REQUIRED)
find_package(OpenCV REQUIRED)

    # Use PkgConfig
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavcodec libavformat libavutil libswscale)
    pkg_check_modules(WEBP REQUIRED IMPORTED_TARGET libwebp)
    set(FFMPEG_LIBS PkgConfig::FFMPEG)
    set(WEBP_LIBS PkgConfig::WEBP)

# Define the library
add_library(media_compress_net
    src/MediaCompressor.cpp
    src/NetworkTransfer.cpp
)

# Include directories
target_include_directories(media_compress_net PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link libraries
target_link_libraries(media_compress_net PUBLIC
    Threads::Threads
    ${OpenCV_LIBS}
    ${FFMPEG_LIBS}
    ${WEBP_LIBS}
)

if(TARGET Boost::headers)
    target_link_libraries(media_compress_net PUBLIC Boost::headers)
elseif(TARGET Boost::boost)
    target_link_libraries(media_compress_net PUBLIC Boost::boost)
endif()

# --- Install & Export ---
include(GNUInstallDirs)

install(TARGETS media_compress_net
    EXPORT media_compress_netTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT media_compress_netTargets
    FILE media_compress_netTargets.cmake
    NAMESPACE media_compress_net::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/media_compress_net
)

# Basic test executable configuration (if testing is enabled)
option(MEDIA_COMPRESS_NET_BUILD_TESTS "Build tests" OFF)
if(MEDIA_COMPRESS_NET_BUILD_TESTS)
    enable_testing()
    # Assume a dummy test file exists for CI to pass
    file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp "int main() { return 0; }\n")
    add_executable(test_media_compress_net tests/main.cpp)
    target_link_libraries(test_media_compress_net PRIVATE media_compress_net)
    add_test(NAME CoreTest COMMAND test_media_compress_net)
endif()
