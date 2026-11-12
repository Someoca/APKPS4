#include "gnm/image.hpp"
#include "gnm/pipeline.hpp"
#include "gnm/profile.hpp"
#include <cassert>
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

    std::cout << "PASS: image, upscale, sharpen, blend, profile e limites\n";
    return 0;
}
