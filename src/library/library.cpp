#include "gnm/library.hpp"
#include <algorithm>

#if defined(__FreeBSD__)
#include <dirent.h>
#include <cstring>
#else
#include <filesystem>
#include <system_error>
#endif

namespace gnm {
std::vector<GameEntry> scanGames(const std::string& root) {
    std::vector<GameEntry> games;
#if defined(__FreeBSD__)
    // Alvo PS4 (OpenOrbis): sem std::filesystem, usa dirent.
    DIR* dir = opendir(root.c_str());
    if (dir) {
        while (dirent* e = readdir(dir)) {
            if (e->d_type != DT_DIR || std::strcmp(e->d_name, ".") == 0 ||
                std::strcmp(e->d_name, "..") == 0) {
                continue;
            }
            games.push_back({e->d_name, root + "/" + e->d_name});
        }
        closedir(dir);
    }
#else
    namespace fs = std::filesystem;
    std::error_code ec;
    for (fs::directory_iterator it(root, ec), end; !ec && it != end; it.increment(ec)) {
        std::error_code statusEc;
        if (it->is_directory(statusEc) && !statusEc) {
            games.push_back({it->path().filename().string(), it->path().string()});
        }
    }
#endif
    std::sort(games.begin(), games.end(),
              [](const GameEntry& a, const GameEntry& b) { return a.name < b.name; });
    return games;
}
}
