#ifndef HTTP_BUFFER_H_INCLUDED
#define HTTP_BUFFER_H_INCLUDED

#include <cstddef>
#include <array>
#include <memory>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "http_response.h"
#include "stream.h"

namespace cld::http {

/* Design goal of cld::http::Buffer is to shield all difference between body
 * cld::http::MakeBuffer helps to create buffers
 */
template <std::size_t S = 4096>
class Buffer {
public:
    Buffer() : data_(), end_(data_.data()) { }

    // return value indicates if the body is unfinished, maintain end_
    virtual void read(transport::Stream &stream) = 0;
    virtual bool finished() = 0;

    std::array<std::byte, S> &data() { return data_; };
    const std::array<std::byte, S> &data() const { return data_; };

    std::size_t validCount() { return end_ - data_.data(); }

    std::byte *begin() { return data_.begin(); }
    std::byte *end() { return end_; }
    const std::byte *begin() const { return data_.begin(); }
    const std::byte *end() const { return end_; }

protected:
    std::array<std::byte, S> data_;
    std::byte *end_;
};

template <std::size_t S = 4096>
class NoLengthBuffer : public Buffer<S> {
public:
    NoLengthBuffer() : finished_(false) { }

    void read(transport::Stream &stream) override {
        std::size_t read_count = stream.read(this->data_.data(), this->data_.size());
        this->end_ = this->data_.data() + read_count;
        if (read_count == 0)
            finished_ = true;
    }

    bool finished() override {
        return finished_;
    }

private:
    bool finished_;
};

template <std::size_t S = 4096>
class FixLengthBuffer : public Buffer<S> {
public:
    explicit FixLengthBuffer(std::size_t size) : remain_(size) {}
    void read(transport::Stream &stream) override {
        std::size_t read_count = stream.read(this->data_.data(),
            std::min(remain_, this->data_.size()));
        this->end_ = this->data_.data() + read_count;
        remain_ -= read_count;
    }

    bool finished() override {
        return remain_ == 0;
    }

private:
    std::size_t remain_;
};

template <std::size_t S = 4096>
class ChunkedBuffer : public Buffer<S> {
public:
    ChunkedBuffer() : chunk_remain_(-1) {}
    void read(transport::Stream &stream) override {
        std::vector<std::byte> temp;
        std::string chunk_head;
        char c;
        auto c_buf = reinterpret_cast<std::byte *>(&c);
        if (chunk_remain_ == -1) {
            // wait for next chunk
            while (stream.read(c_buf, 1) != 0) {
                if (!chunk_head.empty() && c == '\r') {
                    break;
                }
                if (std::isblank(c)) {
                    if (chunk_head.empty())
                        continue;
                    else
                        throw std::runtime_error("Chunk error");
                }
                chunk_head.push_back(c);
            }
            if (stream.read(c_buf, 1) != 0 && c == '\n') {
                std::istringstream ss(chunk_head);
                ss >> std::setbase(16) >> chunk_remain_;
            } else {
                throw std::runtime_error("Chunk error");
            }
        }

        if (chunk_remain_ > 0) {
            std::size_t read_count = stream.read(
                    this->data_.data(), std::min(static_cast<std::size_t>(chunk_remain_), this->data_.size()));
            this->end_ = this->data_.data() + read_count;
            chunk_remain_ -= read_count;
            if (chunk_remain_ == 0) {
                chunk_remain_ = -1;
            }
        }

        if (chunk_remain_ == 0) {
            this->end_ = this->data_.data();
        }

        // if don't finish chunk header
        this->end_ = this->data_.data(); // no data
    }

    bool finished() override {
        return chunk_remain_ == 0;
    }

private:
    long long chunk_remain_;
};

template <std::size_t S = 4096> static inline std::shared_ptr<Buffer<S>>
CreateBuffer(const Response &response) {
    if (response["Transfer-Encoding"] == "chunked") {
        return std::make_shared<ChunkedBuffer<S>>();
    }
    if (response["Connection"] == "keep-alive") {
        std::size_t length;
        if (std::istringstream(response["Content-Length"]) >> length)
            return std::make_shared<FixLengthBuffer<S>>(length);
        else
            throw std::runtime_error("Failed to get content length");
    } else {
        return std::make_shared<NoLengthBuffer<S>>();
    }
}

} // cld::http

#endif
