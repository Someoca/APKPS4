#include "gnm/image.hpp"
#include "gnm/pipeline.hpp"
#include "gnm/profile.hpp"
#include <iostream>

int main() {
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
    std::cout << "Nucleo experimental executado com sucesso.\n";
    return output.valid() ? 0 : 1;
}
