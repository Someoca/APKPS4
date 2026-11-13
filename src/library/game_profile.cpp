#include "gnm/game_profile.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace gnm {
namespace {
std::string trim(const std::string& s) {
    const char* ws = " \t\r\n";
    const auto b = s.find_first_not_of(ws);
    if (b == std::string::npos) return "";
    return s.substr(b, s.find_last_not_of(ws) - b + 1);
}

bool validTitleId(const std::string& id) {
    if (id.empty() || id.size() > 12) return false;
    return std::all_of(id.begin(), id.end(), [](unsigned char c) {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
    });
}

uint32_t clampU(long v, uint32_t lo, uint32_t hi) {
    return static_cast<uint32_t>(std::max<long>(lo, std::min<long>(hi, v)));
}
}

GameProfile sanitize(GameProfile p) {
    p.sharpness = std::max(0.0f, std::min(1.0f, p.sharpness));
    p.outputWidth = clampU(p.outputWidth, 320, 7680);
    p.outputHeight = clampU(p.outputHeight, 180, 4320);
    p.targetFps = clampU(p.targetFps, 15, 120);
    p.gameSpeed = std::max(0.25f, std::min(4.0f, p.gameSpeed));
    return p;
}

GameProfile defaultProfile(const std::string& titleId) {
    GameProfile p;
    p.titleId = titleId;
    return p;
}

ProfileMap loadProfiles(const std::string& path) {
    ProfileMap profiles;
    std::ifstream in(path);
    if (!in) return profiles;

    GameProfile* current = nullptr;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;
        if (line.front() == '[' && line.back() == ']') {
            const std::string id = trim(line.substr(1, line.size() - 2));
            current = nullptr;
            if (validTitleId(id)) {
                current = &profiles[id];
                *current = defaultProfile(id);
            }
            continue;
        }
        const auto eq = line.find('=');
        if (!current || eq == std::string::npos) continue;
        const std::string key = trim(line.substr(0, eq));
        const std::string value = trim(line.substr(eq + 1));
        if (key == "sharpness") current->sharpness = std::strtof(value.c_str(), nullptr);
        else if (key == "width") current->outputWidth = clampU(std::strtol(value.c_str(), nullptr, 10), 0, 100000);
        else if (key == "height") current->outputHeight = clampU(std::strtol(value.c_str(), nullptr, 10), 0, 100000);
        else if (key == "fps") current->targetFps = clampU(std::strtol(value.c_str(), nullptr, 10), 0, 100000);
        else if (key == "speed") current->gameSpeed = std::strtof(value.c_str(), nullptr);
    }
    for (auto& entry : profiles) entry.second = sanitize(entry.second);
    return profiles;
}

bool saveProfiles(const ProfileMap& profiles, const std::string& path) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) return false;
    out << "# GNM Scale - perfis por jogo\n";
    for (const auto& entry : profiles) {
        if (!validTitleId(entry.first)) continue;
        const GameProfile p = sanitize(entry.second);
        out << "\n[" << entry.first << "]\n"
            << "sharpness=" << p.sharpness << "\n"
            << "width=" << p.outputWidth << "\n"
            << "height=" << p.outputHeight << "\n"
            << "fps=" << p.targetFps << "\n"
            << "speed=" << p.gameSpeed << "\n";
    }
    return static_cast<bool>(out);
}

std::string profilesToJson(const ProfileMap& profiles) {
    std::ostringstream os;
    os << "{\n  \"format\": \"gnm-profiles\",\n  \"version\": 1,\n  \"games\": [";
    bool first = true;
    for (const auto& entry : profiles) {
        if (!validTitleId(entry.first)) continue;
        const GameProfile p = sanitize(entry.second);
        os << (first ? "\n" : ",\n") << "    {\"titleId\": \"" << entry.first
           << "\", \"sharpness\": " << p.sharpness << ", \"width\": " << p.outputWidth
           << ", \"height\": " << p.outputHeight << ", \"fps\": " << p.targetFps
           << ", \"speed\": " << p.gameSpeed << "}";
        first = false;
    }
    os << (first ? "" : "\n  ") << "]\n}\n";
    return os.str();
}

bool exportProfilesJson(const ProfileMap& profiles, const std::string& path) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) return false;
    out << profilesToJson(profiles);
    return static_cast<bool>(out);
}
}
