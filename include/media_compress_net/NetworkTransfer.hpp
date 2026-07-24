#ifndef MEDIA_COMPRESS_NET_NETWORKTRANSFER_HPP
#define MEDIA_COMPRESS_NET_NETWORKTRANSFER_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <memory>
#include <future>

namespace mcn {

/**
 * @brief Base interface for network transfers.
 */
class NetworkTransfer {
public:
    virtual ~NetworkTransfer() = default;

    // Callback for transfer progress (bytes_transferred, total_bytes)
    using ProgressCallback = std::function<void(std::size_t, std::size_t)>;

    virtual void set_progress_callback(ProgressCallback cb) = 0;
    virtual std::future<bool> send_data(const std::vector<uint8_t>& data) = 0;
    virtual std::future<bool> send_file(const std::string& file_path) = 0;
};

/**
 * @brief LAN Transfer implementation using direct Boost.Asio TCP sockets.
 */
class LANTransfer : public NetworkTransfer {
public:
    LANTransfer(const std::string& target_ip, uint16_t port);
    ~LANTransfer() override;

    void set_progress_callback(ProgressCallback cb) override;
    std::future<bool> send_data(const std::vector<uint8_t>& data) override;
    std::future<bool> send_file(const std::string& file_path) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

/**
 * @brief WAN Transfer implementation using chunked HTTP/REST uploads.
 */
class WANTransfer : public NetworkTransfer {
public:
    WANTransfer(const std::string& server_url, const std::string& auth_token);
    ~WANTransfer() override;

    void set_progress_callback(ProgressCallback cb) override;
    std::future<bool> send_data(const std::vector<uint8_t>& data) override;
    std::future<bool> send_file(const std::string& file_path) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

/**
 * @brief P2P Transfer implementation using STUN/ICE and UDP.
 */
class P2PTransfer : public NetworkTransfer {
public:
    P2PTransfer(const std::string& signaling_server_url);
    ~P2PTransfer() override;

    void set_progress_callback(ProgressCallback cb) override;
    std::future<bool> send_data(const std::vector<uint8_t>& data) override;
    std::future<bool> send_file(const std::string& file_path) override;

    // Initiates P2P connection
    std::future<bool> connect_to_peer(const std::string& peer_id);

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace mcn

#endif // MEDIA_COMPRESS_NET_NETWORKTRANSFER_HPP
