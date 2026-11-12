#include "gnm/image.hpp"
#include <limits>
#include <stdexcept>

namespace gnm {
Image::Image(uint32_t width, uint32_t height)
    : width_(width), height_(height) {
    const size_t maxSize = pixels_.max_size();

    if (height != 0 &&
        static_cast<size_t>(width) > maxSize / static_cast<size_t>(height) / 4) {
        throw std::length_error("Image dimensions are too large");
    }

    const size_t count = static_cast<size_t>(width) *
                         static_cast<size_t>(height) * 4;
    pixels_.resize(count, 0);
}

uint32_t Image::width() const { return width_; }
uint32_t Image::height() const { return height_; }

bool Image::valid() const {
    if (width_ == 0 || height_ == 0) return false;

    const size_t maxSize = pixels_.max_size();
    if (static_cast<size_t>(width_) >
        maxSize / static_cast<size_t>(height_) / 4) return false;

    return pixels_.size() ==
        static_cast<size_t>(width_) * height_ * 4;
}

uint8_t* Image::pixel(uint32_t x, uint32_t y) {
    if (x >= width_ || y >= height_) return nullptr;
    return &pixels_[(static_cast<size_t>(y) * width_ + x) * 4];
}

const uint8_t* Image::pixel(uint32_t x, uint32_t y) const {
    if (x >= width_ || y >= height_) return nullptr;
    return &pixels_[(static_cast<size_t>(y) * width_ + x) * 4];
}
}
