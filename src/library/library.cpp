#include "gnm/library.hpp"
#include <algorithm>
#include <filesystem>
#include <system_error>

namespace gnm {
std::vector<GameEntry> scanGames(const std::string& root) {
    namespace fs = std::filesystem;
    std::vector<GameEntry> games;
    std::error_code ec;
    for (fs::directory_iterator it(root, ec), end; !ec && it != end; it.increment(ec)) {
        std::error_code statusEc;
        if (it->is_directory(statusEc) && !statusEc) {
            games.push_back({it->path().filename().string(), it->path().string()});
        }
    }
    std::sort(games.begin(), games.end(),
              [](const GameEntry& a, const GameEntry& b) { return a.name < b.name; });
    return games;
}
}
