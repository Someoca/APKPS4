// Ponto de entrada para o eboot do PS4 (toolchain OpenOrbis). Nao e compilado no build do host.
#include "gnm/frame_limiter.hpp"
#include "gnm/image.hpp"
#include "gnm/library.hpp"
#include "gnm/pipeline.hpp"
#include <cstdio>

// Pasta onde o PS4 guarda os jogos instalados (um diretorio por TITLE_ID).
static const char* kGamesRoot = "/user/app";

int main() {
    const gnm::PipelineConfig config;
    const auto games = gnm::scanGames(kGamesRoot);
    std::printf("GNM SCALE PS4 - biblioteca (%u jogos):\n", static_cast<unsigned>(games.size()));
    for (size_t i = 0; i < games.size(); ++i) {
        std::printf("  [%u] %s\n", static_cast<unsigned>(i), games[i].name.c_str());
    }

    gnm::Image input(320, 180);
    auto output = gnm::upscaleBilinear(input, 640, 360);
    gnm::sharpen(output, 0.35f);
    std::printf("Pipeline: %ux%u, alvo %u FPS, velocidade %.1fx\n",
                output.width(), output.height(), config.targetFps, config.gameSpeed);

    gnm::FrameLimiter limiter(config.targetFps, config.gameSpeed);
    for (;;) limiter.wait();
}
