#include "gnm/image.hpp"
#include "gnm/game_profile.hpp"
#include "gnm/library.hpp"
#include "gnm/frame_limiter.hpp"
#include "gnm/pipeline.hpp"
#include "gnm/profile.hpp"
#include "gnm/render.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

// Uso: gnm_scale [pasta_dos_jogos [indice]]
//      gnm_scale --enhance entrada.ppm saida.ppm [nitidez 0..1]
//      gnm_scale --profile perfis.ini TITLE_ID [sharpness=0.5] [width=1920] [height=1080] [fps=60] [speed=1.0]
//      gnm_scale --export perfis.ini perfis.json
int main(int argc, char** argv) {
    if (argc >= 4 && std::string(argv[1]) == "--profile") {
        auto profiles = gnm::loadProfiles(argv[2]);
        const std::string id = argv[3];
        auto profile = profiles.count(id) ? profiles[id] : gnm::defaultProfile(id);
        for (int i = 4; i < argc; ++i) {
            const std::string kv = argv[i];
            const auto eq = kv.find('=');
            if (eq == std::string::npos) continue;
            const std::string k = kv.substr(0, eq);
            const float v = std::strtof(kv.c_str() + eq + 1, nullptr);
            if (k == "sharpness") profile.sharpness = v;
            else if (k == "width") profile.outputWidth = static_cast<uint32_t>(v);
            else if (k == "height") profile.outputHeight = static_cast<uint32_t>(v);
            else if (k == "fps") profile.targetFps = static_cast<uint32_t>(v);
            else if (k == "speed") profile.gameSpeed = v;
        }
        profiles[id] = gnm::sanitize(profile);
        if (!gnm::saveProfiles(profiles, argv[2])) {
            std::cerr << "Falha ao gravar " << argv[2] << " (TITLE_ID invalido?)\n";
            return 2;
        }
        std::cout << "Perfil salvo: " << id << "\n";
        return 0;
    }
    if (argc >= 4 && std::string(argv[1]) == "--export") {
        const auto profiles = gnm::loadProfiles(argv[2]);
        if (!gnm::exportProfilesJson(profiles, argv[3])) {
            std::cerr << "Falha ao exportar " << argv[3] << "\n";
            return 2;
        }
        std::cout << "Exportados " << profiles.size() << " perfis para " << argv[3] << "\n";
        return 0;
    }
    if (argc >= 4 && std::string(argv[1]) == "--enhance") {
        const auto in = gnm::loadPpm(argv[2]);
        if (!in.valid()) {
            std::cerr << "Falha ao ler PPM: " << argv[2] << "\n";
            return 2;
        }
        gnm::EnhanceConfig cfg;
        if (argc > 4) cfg.sharpness = std::stof(argv[4]);
        const auto out = gnm::enhance(in, cfg);
        if (!gnm::savePpm(out, argv[3])) {
            std::cerr << "Falha ao gravar PPM: " << argv[3] << "\n";
            return 2;
        }
        std::cout << "Imagem melhorada: " << in.width() << "x" << in.height() << " -> "
                  << out.width() << "x" << out.height() << "\n";
        return 0;
    }
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
