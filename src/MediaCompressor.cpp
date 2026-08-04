#include "media_compress_net/MediaCompressor.hpp"
#include <iostream>
#include <stdexcept>

// FFmpeg includes (extern "C")
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

// OpenCV for Image Compression (stub/example)
#include <opencv2/opencv.hpp>

namespace mcn {

struct MediaCompressor::Impl {
    // Internal state can go here
};

MediaCompressor::MediaCompressor() : pimpl_(std::make_unique<Impl>()) {
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100)
    av_register_all(); // Deprecated in newer FFmpeg, but good for older versions
#endif
}

MediaCompressor::~MediaCompressor() = default;

std::vector<uint8_t> MediaCompressor::compress_image(const std::vector<uint8_t>& input_data, const ImageCompressConfig& config) {
    // Example OpenCV + libwebp stub implementation
    std::vector<uint8_t> output;
    try {
        cv::Mat raw_img = cv::imdecode(input_data, cv::IMREAD_COLOR);
        if (raw_img.empty()) {
            throw std::runtime_error("Failed to decode input image.");
        }
        
        cv::Mat resized;
        cv::resize(raw_img, resized, cv::Size(config.target_width, config.target_height));

        std::vector<int> compression_params;
        compression_params.push_back(cv::IMWRITE_WEBP_QUALITY);
        compression_params.push_back(config.quality);

        cv::imencode(".webp", resized, output, compression_params);
    } catch (const std::exception& e) {
        std::cerr << "Image compression error: " << e.what() << std::endl;
    }
    return output;
}

bool MediaCompressor::compress_video(const std::string& input_path, const std::string& output_path, const VideoCompressConfig& config) {
    std::cout << "Starting video compression using FFmpeg CLI wrapper..." << std::endl;
    
    // Construct the FFmpeg command
    std::string command = "ffmpeg -y -i \"" + input_path + "\" ";
    
    // Apply video configurations
    command += "-vcodec libx264 ";
    command += "-preset " + config.preset + " ";
    command += "-crf " + std::to_string(config.crf) + " ";
    command += "-vf scale=" + std::to_string(config.target_width) + ":" + std::to_string(config.target_height) + " ";
    command += "-r " + std::to_string(config.fps) + " ";
    
    // Copy audio by default
    command += "-c:a copy ";
    
    // Output path
    command += "\"" + output_path + "\"";
    
    std::cout << "Executing: " << command << std::endl;
    
    // Execute the command synchronously
    int result = std::system(command.c_str());
    
    if (result != 0) {
        std::cerr << "FFmpeg compression failed with exit code: " << result << std::endl;
        return false;
    }
    
    std::cout << "Successfully compressed video to: " << output_path << std::endl;
    return true;
}

std::future<std::vector<uint8_t>> MediaCompressor::compress_image_async(std::vector<uint8_t> input_data, ImageCompressConfig config) {
    return std::async(std::launch::async, [this, data = std::move(input_data), config]() {
        return compress_image(data, config);
    });
}

std::future<bool> MediaCompressor::compress_video_async(std::string input_path, std::string output_path, VideoCompressConfig config) {
    return std::async(std::launch::async, [this, in = std::move(input_path), out = std::move(output_path), config]() {
        return compress_video(in, out, config);
    });
}

} // namespace mcn
