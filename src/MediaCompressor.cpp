#include "media_compress_net/MediaCompressor.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>

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
    // Snippet demonstrating FFmpeg video compression setup
    const AVCodec* codec = avcodec_find_encoder_by_name("libx264");
    if (!codec) {
        std::cerr << "Codec libx264 not found." << std::endl;
        return false;
    }

    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        std::cerr << "Could not allocate video codec context." << std::endl;
        return false;
    }

    // Configure the context
    codec_ctx->width = config.target_width;
    codec_ctx->height = config.target_height;
    codec_ctx->time_base = {1, config.fps};
    codec_ctx->framerate = {config.fps, 1};
    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

    // x264 specific options
    AVDictionary* opt = nullptr;
    av_dict_set(&opt, "preset", config.preset.c_str(), 0);
    av_dict_set(&opt, "crf", std::to_string(config.crf).c_str(), 0);
    av_dict_set(&opt, "profile", "main", 0);
    av_dict_set(&opt, "tune", "zerolatency", 0);

    if (avcodec_open2(codec_ctx, codec, &opt) < 0) {
        std::cerr << "Could not open codec." << std::endl;
        avcodec_free_context(&codec_ctx);
        av_dict_free(&opt);
        return false;
    }

    // Note: Full implementation would require setting up AVFormatContext for I/O,
    // reading frames from input_path via av_read_frame/avcodec_send_packet,
    // scaling with sws_scale, encoding with avcodec_send_frame/avcodec_receive_packet,
    // and muxing to output_path.
    
    std::cout << "Successfully configured video codec for: " << output_path << std::endl;

    av_dict_free(&opt);
    avcodec_free_context(&codec_ctx);
    return true; // Return true as a stub indicating successful setup
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
