#pragma once
#include <cstdint>
#include <vector>

namespace gnm {
class Image {
public:
    Image() = default;
    Image(uint32_t width, uint32_t height);

    uint32_t width() const;
    uint32_t height() const;
    bool valid() const;
    uint8_t* pixel(uint32_t x, uint32_t y);
    const uint8_t* pixel(uint32_t x, uint32_t y) const;

private:
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    std::vector<uint8_t> pixels_;
};
}
