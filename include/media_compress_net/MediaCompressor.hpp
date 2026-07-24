#ifndef MEDIA_COMPRESS_NET_MEDIACOMPRESSOR_HPP
#define MEDIA_COMPRESS_NET_MEDIACOMPRESSOR_HPP

#include <vector>
#include <cstdint>
#include <string>
#include <future>
#include <memory>

namespace mcn {

struct VideoCompressConfig {
    int target_width = 1920;
    int target_height = 1080;
    int crf = 23;                // Constant Rate Factor (lower is better quality, 23 is default)
    std::string preset = "medium"; // e.g., "fast", "medium", "slow"
    int fps = 30;
};

struct ImageCompressConfig {
    int target_width = 1920;
    int target_height = 1080;
    int quality = 80;            // WebP quality (0-100)
};

class MediaCompressor {
public:
    MediaCompressor();
    ~MediaCompressor();

    // Disable copy semantics
    MediaCompressor(const MediaCompressor&) = delete;
    MediaCompressor& operator=(const MediaCompressor&) = delete;

    /**
     * @brief Compresses an image synchronously using libwebp/OpenCV.
     * @param input_data Raw image data (e.g., JPEG/PNG bytes)
     * @param config Compression configuration
     * @return Compressed image bytes (WebP format)
     */
    std::vector<uint8_t> compress_image(const std::vector<uint8_t>& input_data, const ImageCompressConfig& config);

    /**
     * @brief Compresses a video synchronously using FFmpeg.
     * @param input_path Path to the input video file
     * @param output_path Path for the compressed output video
     * @param config Compression configuration
     * @return True if successful, false otherwise
     */
    bool compress_video(const std::string& input_path, const std::string& output_path, const VideoCompressConfig& config);

    /**
     * @brief Asynchronous image compression
     */
    std::future<std::vector<uint8_t>> compress_image_async(std::vector<uint8_t> input_data, ImageCompressConfig config);

    /**
     * @brief Asynchronous video compression
     */
    std::future<bool> compress_video_async(std::string input_path, std::string output_path, VideoCompressConfig config);

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace mcn

#endif // MEDIA_COMPRESS_NET_MEDIACOMPRESSOR_HPP
