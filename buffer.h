#ifndef CLD_BUFFER_H_INCLUDED
#define CLD_BUFFER_H_INCLUDED

#include <cstddef>
#include <string>
#include <stdexcept>
#include <memory>
#include <algorithm>

template <typename T, typename Alloc = std::allocator<T>>
class Buffer {
public:
    Buffer(std::size_t s, const Alloc &alloc = Alloc()) : allocator(alloc) {
        if (s < 1) {
            throw std::logic_error(std::string("Invalid buffer size ") + std::to_string(s));
        }
        size_ = s;
        valid_ = 0;
        data_ = std::allocator_traits<Alloc>::allocate(allocator, size_);
    }
    Buffer(const Buffer &other) = delete;
    Buffer(Buffer &&other) noexcept {
        size_ = other.size_;
        data_ = other.data_;
        valid_ = other.valid_;
        other.data_ = nullptr;
        other.valid_ = 0;
        other.size_ = 0;
        std::swap(allocator, other.allocator);
    }
    Buffer &operator=(const Buffer &other) = delete;
    Buffer &operator=(Buffer &&other) noexcept {
        size_ = other.size_;
        data_ = other.data_;
        valid_ = other.valid_;
        other.data_ = nullptr;
        other.valid_ = 0;
        other.size_ = 0;
        std::swap(allocator, other.allocator);
        return *this;
    }
    ~Buffer() noexcept { std::allocator_traits<Alloc>::deallocate(allocator, data_, size_); }

    void setValid(std::size_t valid) { valid_ = valid; }

    operator T *() { return data_; }
    std::size_t size() { return size_; }
    std::size_t valid() { return valid_; }
    T *data() { return data_; }

private:
    std::size_t valid_;
    std::size_t size_;
    T *data_;
    
    Alloc allocator;
};

#endif
