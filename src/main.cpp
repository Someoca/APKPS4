#include "gnm/image.hpp"
#include "gnm/library.hpp"
#include "gnm/frame_limiter.hpp"
#include "gnm/pipeline.hpp"
#include "gnm/profile.hpp"
#include <iostream>
#include <string>

// Uso: gnm_scale [pasta_dos_jogos [indice]]
int main(int argc, char** argv) {
    if (argc > 1) {
        const auto games = gnm::scanGames(argv[1]);
        std::cout << "Biblioteca (" << games.size() << " jogos):\n";
        for (size_t i = 0; i < games.size(); ++i) {
            std::cout << "  [" << i << "] " << games[i].name << "\n";
        }
        if (argc > 2) {
            size_t index = 0;
            try {
                index = std::stoul(argv[2]);
            } catch (...) {
                index = games.size();
            }
            if (index >= games.size()) {
                std::cerr << "Jogo invalido: " << argv[2] << "\n";
                return 2;
            }
            std::cout << "Selecionado: " << games[index].name
                      << " (melhoria de imagem alvo 60 FPS)\n";
        }
    }
    const auto profile = gnm::getProfile(gnm::Ps4Model::Unknown);
    gnm::Image input(320, 180);

    for (uint32_t y = 0; y < input.height(); ++y) {
        for (uint32_t x = 0; x < input.width(); ++x) {
            auto* p = input.pixel(x, y);
            p[0] = static_cast<unsigned char>(x % 256);
            p[1] = static_cast<unsigned char>(y % 256);
            p[2] = 128;
            p[3] = 255;
        }
    }

    auto output = gnm::upscaleBilinear(input, 640, 360);
    gnm::sharpen(output, 0.35f);

    std::cout << "GNM SCALE PS4 - Enhanced Edition\n";
    std::cout << "Perfil: " << profile.name << "\n";
    std::cout << "Imagem de teste: " << output.width()
              << "x" << output.height() << "\n";
    const gnm::PipelineConfig config;
    std::cout << "Alvo: " << config.targetFps << " FPS, velocidade "
              << config.gameSpeed << "x (original)\n";
    gnm::FrameLimiter limiter(config.targetFps, config.gameSpeed);
    for (int frame = 0; frame < 3; ++frame) limiter.wait();
    std::cout << "Passo por frame: " << limiter.deltaSeconds() << "s\n";
    std::cout << "Nucleo experimental executado com sucesso.\n";
    return output.valid() ? 0 : 1;
}
