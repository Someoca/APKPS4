#pragma once
#include "gnm/image.hpp"
#include <cstdint>

namespace gnm {
struct PipelineConfig {
    uint32_t outputWidth = 0;
    uint32_t outputHeight = 0;
    float sharpenStrength = 0.0f;
    float temporalWeight = 0.0f;
    uint32_t targetFps = 60;
    float gameSpeed = 1.0f; // 1.0 = velocidade original do jogo
};

Image upscaleBilinear(const Image& input, uint32_t width, uint32_t height);
void sharpen(Image& image, float strength);
Image temporalBlend(const Image& current, const Image& previous, float previousWeight);
}
