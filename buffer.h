#ifndef CLD_BUFFER_H_INCLUDED
#define CLD_BUFFER_H_INCLUDED

#include <cstddef>
#include <string>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <array>

namespace cld {

template <typename T, std::size_t S = 4096>
class Buffer : public std::array<T, S> {
public:
    virtual std::size_t maxRead() const { return size(); }

    virtual void clear() { valid_ = 0; }
    virtual void makeValid(std::size_t valid) {
        if (valid > this->size())
            throw std::logic_error("Valid data number exceed size");
        this->valid_ = valid;
    }

    std::size_t size() const { return data_.size(); }
    T *data() { return data_.data(); }
    T *begin() { return data_.begin(); }
    T *end()   { return data_.begin() + valid_; }
    const T *begin() const { return data_.begin(); }
    const T *end()   const { return data_.begin() + valid_; }
    const T *cbegin() const { return data_.begin(); }
    const T *cend()  const { return data_.begin() + valid_; }

private:
    std::size_t valid_ = 0;
    std::array<T, S> data_;
};

template <typename T, std::size_t S = 4096>
class SessionBuffer : public Buffer<T, S> {
public:
    SessionBuffer(std::size_t expected) : remain_(expected) {};
    virtual std::size_t maxRead() const override {
        if (remain_ < this->size())
            return remain_;
        else
            return this->size();
    }
    virtual void makeValid(std::size_t valid) override {
        if (valid > remain_)
            throw std::logic_error("New data exceed remain");
        Buffer<T, S>::makeValid(valid);
        remain_ -= valid;
    }

    bool finished() const { return remain_ == 0; }
    std::size_t remain() { return remain_; }

private:
    std::size_t remain_;
};

} // namespace cld

#endif
