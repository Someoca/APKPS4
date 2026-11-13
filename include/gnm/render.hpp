#pragma once
#include "gnm/image.hpp"
#include <string>

namespace gnm {
// Upscale bicubico (Catmull-Rom), mais nitido que o bilinear.
Image upscaleBicubic(const Image& input, uint32_t width, uint32_t height);

// Contrast Adaptive Sharpening (CAS, estilo AMD FidelityFX). strength em [0, 1].
void sharpenCas(Image& image, float strength);

struct EnhanceConfig {
    uint32_t outputWidth = 0;  // 0 = 2x da entrada
    uint32_t outputHeight = 0;
    float sharpness = 0.5f;
};

// Pipeline completo: upscale bicubico seguido de CAS. Mesma logica prevista para o compute shader do PS4.
Image enhance(const Image& input, const EnhanceConfig& config);

// Imagens PPM (P6): formato simples, sem dependencias, usado para imagem e quadros de video.
Image loadPpm(const std::string& path);
bool savePpm(const Image& image, const std::string& path);
}
