#include "gnm/pipeline.hpp"
#include <algorithm>
#include <cmath>

namespace gnm {
namespace {
uint8_t clampByte(float value) {
    return static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, value)));
}
}

Image upscaleBilinear(const Image& input, uint32_t width, uint32_t height) {
    if (!input.valid() || width == 0 || height == 0) return {};

    Image output(width, height);

    for (uint32_t y = 0; y < height; ++y) {
        const float sy = (static_cast<float>(y) + 0.5f) *
                         input.height() / height - 0.5f;
        const float cy = std::max(0.0f, sy);
        const uint32_t y0 = std::min(static_cast<uint32_t>(cy), input.height() - 1);
        const uint32_t y1 = std::min(y0 + 1, input.height() - 1);
        const float fy = cy - y0;

        for (uint32_t x = 0; x < width; ++x) {
            const float sx = (static_cast<float>(x) + 0.5f) *
                             input.width() / width - 0.5f;
            const float cx = std::max(0.0f, sx);
            const uint32_t x0 = std::min(static_cast<uint32_t>(cx), input.width() - 1);
            const uint32_t x1 = std::min(x0 + 1, input.width() - 1);
            const float fx = cx - x0;

            const uint8_t* p00 = input.pixel(x0, y0);
            const uint8_t* p10 = input.pixel(x1, y0);
            const uint8_t* p01 = input.pixel(x0, y1);
            const uint8_t* p11 = input.pixel(x1, y1);
            uint8_t* dst = output.pixel(x, y);

            for (int c = 0; c < 4; ++c) {
                const float top = p00[c] * (1.0f - fx) + p10[c] * fx;
                const float bottom = p01[c] * (1.0f - fx) + p11[c] * fx;
                dst[c] = clampByte(top * (1.0f - fy) + bottom * fy);
            }
        }
    }
    return output;
}

void sharpen(Image& image, float strength) {
    if (!image.valid() || strength <= 0.0f) return;

    strength = std::min(strength, 2.0f);
    Image source = image;

    for (uint32_t y = 1; y + 1 < image.height(); ++y) {
        for (uint32_t x = 1; x + 1 < image.width(); ++x) {
            const uint8_t* center = source.pixel(x, y);
            const uint8_t* left = source.pixel(x - 1, y);
            const uint8_t* right = source.pixel(x + 1, y);
            const uint8_t* up = source.pixel(x, y - 1);
            const uint8_t* down = source.pixel(x, y + 1);
            uint8_t* dst = image.pixel(x, y);

            for (int c = 0; c < 3; ++c) {
                const float average = (left[c] + right[c] + up[c] + down[c]) / 4.0f;
                dst[c] = clampByte(center[c] + strength * (center[c] - average));
            }
        }
    }
}

Image temporalBlend(const Image& current, const Image& previous, float weight) {
    if (!current.valid() || !previous.valid() ||
        current.width() != previous.width() ||
        current.height() != previous.height()) return {};

    weight = std::max(0.0f, std::min(1.0f, weight));
    Image output(current.width(), current.height());

    for (uint32_t y = 0; y < current.height(); ++y) {
        for (uint32_t x = 0; x < current.width(); ++x) {
            const uint8_t* a = current.pixel(x, y);
            const uint8_t* b = previous.pixel(x, y);
            uint8_t* dst = output.pixel(x, y);
            for (int c = 0; c < 4; ++c)
                dst[c] = clampByte(a[c] * (1.0f - weight) + b[c] * weight);
        }
    }
    return output;
}
}
