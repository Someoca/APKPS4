// Ponto de entrada para o eboot do PS4 (toolchain OpenOrbis). Nao e compilado no build do host.
#include "gnm/frame_limiter.hpp"
#include "gnm/image.hpp"
#include "gnm/pipeline.hpp"
#include <cstdio>

int main() {
    const gnm::PipelineConfig config;
    gnm::Image input(320, 180);
    auto output = gnm::upscaleBilinear(input, 640, 360);
    gnm::sharpen(output, 0.35f);
    std::printf("GNM SCALE PS4: %ux%u, alvo %u FPS, velocidade %.1fx\n",
                output.width(), output.height(), config.targetFps, config.gameSpeed);
    gnm::FrameLimiter limiter(config.targetFps, config.gameSpeed);
    for (;;) limiter.wait();
}
