#include "gnm/image.hpp"
#include "gnm/frame_limiter.hpp"
#include "gnm/game_profile.hpp"
#include "gnm/library.hpp"
#include "gnm/pipeline.hpp"
#include "gnm/profile.hpp"
#include "gnm/render.hpp"
#include "../plugin/gnm_profile_plugin/source/gnm_profile_parse.h"
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

    gnm::Image flat(8, 8);
    for (uint32_t y = 0; y < 8; ++y)
        for (uint32_t x = 0; x < 8; ++x) {
            uint8_t* p = flat.pixel(x, y);
            p[0] = p[1] = p[2] = 100;
            p[3] = 255;
        }
    auto big = gnm::upscaleBicubic(flat, 16, 16);
    assert(big.width() == 16 && big.pixel(7, 7)[0] == 100);
    gnm::sharpenCas(big, 1.0f);
    assert(big.pixel(7, 7)[0] == 100);

    gnm::Image edge(8, 8);
    for (uint32_t y = 0; y < 8; ++y)
        for (uint32_t x = 0; x < 8; ++x) {
            uint8_t* p = edge.pixel(x, y);
            p[0] = p[1] = p[2] = x < 4 ? 80 : 160;
            p[3] = 255;
        }
    auto sharp = edge;
    gnm::sharpenCas(sharp, 1.0f);
    assert(sharp.pixel(3, 4)[0] < 80 && sharp.pixel(4, 4)[0] > 160);
    auto none = edge;
    gnm::sharpenCas(none, 0.0f);
    assert(none.pixel(3, 4)[0] == 80);

    gnm::EnhanceConfig ec;
    auto enhanced = gnm::enhance(edge, ec);
    assert(enhanced.width() == 16 && enhanced.height() == 16);
    assert(!gnm::enhance(gnm::Image(), ec).valid());

    const auto ppm = std::filesystem::temp_directory_path() / "gnm_test.ppm";
    assert(gnm::savePpm(edge, ppm.string()));
    auto loaded = gnm::loadPpm(ppm.string());
    assert(loaded.valid() && loaded.pixel(5, 2)[0] == 160 && loaded.pixel(0, 0)[3] == 255);
    std::filesystem::remove(ppm);
    assert(!gnm::loadPpm((ppm.parent_path() / "nao_existe.ppm").string()).valid());

    gnm::GameProfile wild = gnm::defaultProfile("CUSA00001");
    wild.sharpness = 9.0f;
    wild.targetFps = 1000;
    wild.outputWidth = 1;
    wild = gnm::sanitize(wild);
    assert(wild.sharpness == 1.0f && wild.targetFps == 120 && wild.outputWidth == 320);

    gnm::ProfileMap profiles;
    gnm::GameProfile gp = gnm::defaultProfile("CUSA00001");
    gp.sharpness = 0.75f;
    gp.targetFps = 30;
    profiles["CUSA00001"] = gp;
    profiles["CUSA00002"] = gnm::defaultProfile("CUSA00002");
    profiles["../ruim"] = gnm::defaultProfile("../ruim");
    const auto ini = std::filesystem::temp_directory_path() / "gnm_test_profiles.ini";
    assert(gnm::saveProfiles(profiles, ini.string()));
    const auto back = gnm::loadProfiles(ini.string());
    std::filesystem::remove(ini);
    assert(back.size() == 2 && back.at("CUSA00001").sharpness == 0.75f &&
           back.at("CUSA00001").targetFps == 30 && back.at("CUSA00002").outputWidth == 1920);
    assert(gnm::loadProfiles((ini.parent_path() / "nao_existe.ini").string()).empty());
    const auto json = gnm::profilesToJson(back);
    assert(json.find("\"format\": \"gnm-profiles\"") != std::string::npos &&
           json.find("\"titleId\": \"CUSA00001\"") != std::string::npos &&
           json.find("ruim") == std::string::npos);
    assert(gnm_profile_fps(json.c_str(), "CUSA00001") == 30 && gnm_profile_fps(json.c_str(), "CUSA00002") == back.at("CUSA00002").targetFps);
    assert(gnm_profile_fps(json.c_str(), "CUSA99999") == -1 && gnm_profile_fps(json.c_str(), "") == -1);

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
