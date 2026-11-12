#pragma once
#include <string>

namespace gnm {
enum class Ps4Model {
    Unknown,
    Fat,
    Slim,
    Pro
};

struct HardwareProfile {
    Ps4Model model;
    std::string name;
    bool experimental;
};

HardwareProfile getProfile(Ps4Model model);
}
