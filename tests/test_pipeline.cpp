#include "gnm/image.hpp"
#include "gnm/frame_limiter.hpp"
#include "gnm/library.hpp"
#include "gnm/pipeline.hpp"
#include "gnm/profile.hpp"
#include <cassert>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <cstdint>

int main() {
    gnm::Image input(2, 2);
    assert(input.valid());

    for (uint32_t y = 0; y < 2; ++y) {
        for (uint32_t x = 0; x < 2; ++x) {
            auto* p = input.pixel(x, y);
            p[0] = 100; p[1] = 120; p[2] = 140; p[3] = 255;
        }
    }

    auto enlarged = gnm::upscaleBilinear(input, 4, 4);
    assert(enlarged.valid());
    assert(enlarged.width() == 4 && enlarged.height() == 4);

    gnm::sharpen(enlarged, 0.5f);
    auto blended = gnm::temporalBlend(enlarged, enlarged, 0.5f);
    assert(blended.valid());
    assert(blended.width() == 4);

    auto profile = gnm::getProfile(gnm::Ps4Model::Pro);
    assert(profile.model == gnm::Ps4Model::Pro);

    auto invalidBlend = gnm::temporalBlend(input, enlarged, 0.5f);
    assert(!invalidBlend.valid());

    bool oversizedRejected = false;
    try {
        gnm::Image oversized(UINT32_MAX, UINT32_MAX);
    } catch (const std::length_error&) {
        oversizedRejected = true;
    }
    assert(oversizedRejected);

    namespace fs = std::filesystem;
    const auto root = fs::temp_directory_path() / "gnm_test_games";
    fs::remove_all(root);
    fs::create_directories(root / "Zeta");
    fs::create_directories(root / "Alfa");
    const auto games = gnm::scanGames(root.string());
    assert(games.size() == 2 && games[0].name == "Alfa" && games[1].name == "Zeta");
    assert(gnm::scanGames((root / "nao_existe").string()).empty());
    fs::remove_all(root);

    gnm::FrameLimiter limiter(60, 0.5f);
    assert(std::fabs(limiter.deltaSeconds() - (1.0 / 60.0) * 0.5) < 1e-6);
    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < 6; ++i) limiter.wait();
    assert(std::chrono::steady_clock::now() - start >= std::chrono::milliseconds(90));

    std::cout << "PASS: image, upscale, sharpen, blend, profile e limites\n";
    return 0;
}
