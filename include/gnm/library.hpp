#pragma once
#include <string>
#include <vector>

namespace gnm {
struct GameEntry {
    std::string name;
    std::string path;
};

// Cada subpasta de `root` e tratada como um jogo instalado; resultado ordenado por nome.
std::vector<GameEntry> scanGames(const std::string& root);
}
