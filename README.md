# libmedia_compress_net

`libmedia_compress_net` is a high-performance, cross-platform C++ library designed for seamless media compression and network transmission across desktop platforms (Windows, Linux, macOS).

## Features
- **Image & Video Compression**: Utilizes standard C++ libraries (`libwebp`, `OpenCV`, `FFmpeg`) for efficient, configurable media compression.
- **Tri-Mode Network Transfer Engine**:
  - **LAN**: High-throughput direct stream using asynchronous Boost.Asio TCP sockets.
  - **WAN**: Chunked HTTP/REST uploads (stub).
  - **P2P**: Direct STUN/ICE traversal and UDP data streams (stub).
- **Modern C++**: Built with C++17, ensuring type-safe, asynchronous execution via `std::future`.

## Dependencies
- C++17 Compiler (GCC, Clang, or MSVC)
- CMake >= 3.15
- Boost (system)
- OpenCV 4
- FFmpeg (libavcodec, libavformat, libavutil, libswscale)
- libwebp
- PkgConfig (for dependency resolution)

## Build Instructions

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Usage Example

```cpp
#include <media_compress_net/MediaCompressor.hpp>
#include <media_compress_net/NetworkTransfer.hpp>
#include <iostream>

int main() {
    mcn::MediaCompressor compressor;
    
    // Asynchronous Video Compression
    mcn::VideoCompressConfig config;
    config.crf = 26; // High compression
    
    auto future_compress = compressor.compress_video_async("input.mp4", "output.mp4", config);
    if (future_compress.get()) {
        std::cout << "Video compressed successfully!\n";
        
        // LAN Transfer
        mcn::LANTransfer lan("127.0.0.1", 8080);
        auto future_send = lan.send_file("output.mp4");
        
        if (future_send.get()) {
            std::cout << "File transferred successfully via LAN!\n";
        }
    }
    
    return 0;
}
```

## Integration via CMake

You can include this library in your CMake project using `add_subdirectory()` or `find_package()` if installed.

```cmake
add_subdirectory(media_compress_net)
target_link_libraries(your_app PRIVATE media_compress_net)
```

## License
MIT License. See [LICENSE](LICENSE) for details.
