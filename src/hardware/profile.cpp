#include "gnm/profile.hpp"

namespace gnm {
HardwareProfile getProfile(Ps4Model model) {
    switch (model) {
    case Ps4Model::Fat:
        return {model, "PS4 FAT - perfil declarativo", true};
    case Ps4Model::Slim:
        return {model, "PS4 Slim - perfil declarativo", true};
    case Ps4Model::Pro:
        return {model, "PS4 Pro - perfil declarativo", true};
    default:
        return {Ps4Model::Unknown, "Hardware desconhecido", true};
    }
}
}
