#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "gnm/render.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace gnm {
namespace {
uint8_t toByte(float v) {
    return static_cast<uint8_t>(std::lround(std::max(0.0f, std::min(255.0f, v))));
}

float catmullRom(float t) {
    t = std::fabs(t);
    if (t < 1.0f) return 1.5f * t * t * t - 2.5f * t * t + 1.0f;
    if (t < 2.0f) return -0.5f * t * t * t + 2.5f * t * t - 4.0f * t + 2.0f;
    return 0.0f;
}

int clampInt(int v, int lo, int hi) { return std::max(lo, std::min(hi, v)); }
}

Image upscaleBicubic(const Image& input, uint32_t width, uint32_t height) {
    if (!input.valid() || width == 0 || height == 0) return {};
    Image output(width, height);
    const int iw = static_cast<int>(input.width());
    const int ih = static_cast<int>(input.height());

    for (uint32_t y = 0; y < height; ++y) {
        const float sy = (y + 0.5f) * input.height() / height - 0.5f;
        const int y0 = static_cast<int>(std::floor(sy));
        for (uint32_t x = 0; x < width; ++x) {
            const float sx = (x + 0.5f) * input.width() / width - 0.5f;
            const int x0 = static_cast<int>(std::floor(sx));
            float acc[4] = {0, 0, 0, 0};
            float wsum = 0.0f;
            for (int j = -1; j <= 2; ++j) {
                const float wy = catmullRom(sy - (y0 + j));
                for (int i = -1; i <= 2; ++i) {
                    const float w = wy * catmullRom(sx - (x0 + i));
                    const uint8_t* p = input.pixel(clampInt(x0 + i, 0, iw - 1),
                                                   clampInt(y0 + j, 0, ih - 1));
                    for (int c = 0; c < 4; ++c) acc[c] += p[c] * w;
                    wsum += w;
                }
            }
            uint8_t* dst = output.pixel(x, y);
            for (int c = 0; c < 4; ++c) dst[c] = toByte(acc[c] / wsum);
        }
    }
    return output;
}

void sharpenCas(Image& image, float strength) {
    if (!image.valid() || strength <= 0.0f) return;
    strength = std::min(strength, 1.0f);
    const float peak = -1.0f / (8.0f + (5.0f - 8.0f) * strength);
    const Image src = image;
    const int w = static_cast<int>(src.width());
    const int h = static_cast<int>(src.height());

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const uint8_t* c = src.pixel(x, y);
            const uint8_t* n[4] = {
                src.pixel(x, clampInt(y - 1, 0, h - 1)), src.pixel(x, clampInt(y + 1, 0, h - 1)),
                src.pixel(clampInt(x - 1, 0, w - 1), y), src.pixel(clampInt(x + 1, 0, w - 1), y)};
            uint8_t* dst = image.pixel(x, y);
            for (int ch = 0; ch < 3; ++ch) {
                float mn = c[ch] / 255.0f, mx = mn, sum = 0.0f;
                for (const uint8_t* p : n) {
                    const float v = p[ch] / 255.0f;
                    mn = std::min(mn, v);
                    mx = std::max(mx, v);
                    sum += v;
                }
                const float amp = mx > 0.0f ? std::sqrt(std::max(0.0f, std::min(mn, 1.0f - mx) / mx)) : 0.0f;
                const float wt = amp * peak;
                dst[ch] = toByte((wt * sum + c[ch] / 255.0f) / (1.0f + 4.0f * wt) * 255.0f);
            }
        }
    }
}

Image enhance(const Image& input, const EnhanceConfig& config) {
    if (!input.valid()) return {};
    const uint32_t w = config.outputWidth ? config.outputWidth : input.width() * 2;
    const uint32_t h = config.outputHeight ? config.outputHeight : input.height() * 2;
    Image out = upscaleBicubic(input, w, h);
    sharpenCas(out, config.sharpness);
    return out;
}

Image loadPpm(const std::string& path) {
    std::FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return {};
    unsigned w = 0, h = 0, maxv = 0;
    Image img;
    if (std::fscanf(f, "P6 %u %u %u", &w, &h, &maxv) == 3 && maxv == 255 && w > 0 && h > 0 &&
        w <= 16384 && h <= 16384 && std::fgetc(f) != EOF) {
        Image tmp(w, h);
        bool ok = true;
        for (unsigned y = 0; y < h && ok; ++y) {
            for (unsigned x = 0; x < w; ++x) {
                uint8_t rgb[3];
                if (std::fread(rgb, 1, 3, f) != 3) { ok = false; break; }
                uint8_t* p = tmp.pixel(x, y);
                p[0] = rgb[0]; p[1] = rgb[1]; p[2] = rgb[2]; p[3] = 255;
            }
        }
        if (ok) img = tmp;
    }
    std::fclose(f);
    return img;
}

bool savePpm(const Image& image, const std::string& path) {
    if (!image.valid()) return false;
    std::FILE* f = std::fopen(path.c_str(), "wb");
    if (!f) return false;
    std::fprintf(f, "P6\n%u %u\n255\n", image.width(), image.height());
    for (uint32_t y = 0; y < image.height(); ++y) {
        for (uint32_t x = 0; x < image.width(); ++x) std::fwrite(image.pixel(x, y), 1, 3, f);
    }
    return std::fclose(f) == 0;
}
}
