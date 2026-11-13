#pragma once
#include <cstdint>
#include <map>
#include <string>

namespace gnm {
// Perfil de melhoria de imagem de um jogo, identificado pelo TITLE_ID (ex.: CUSA00001).
struct GameProfile {
    std::string titleId;
    float sharpness = 0.5f;       // CAS, 0..1
    uint32_t outputWidth = 1920;  // resolucao alvo
    uint32_t outputHeight = 1080;
    uint32_t targetFps = 60;
    float gameSpeed = 1.0f;       // 1.0 = velocidade original
};

using ProfileMap = std::map<std::string, GameProfile>;

// Valores fora do intervalo sao ajustados para limites validos.
GameProfile sanitize(GameProfile profile);
GameProfile defaultProfile(const std::string& titleId);

// Armazenamento em texto (INI): [TITLE_ID] seguido de chave=valor.
ProfileMap loadProfiles(const std::string& path);
bool saveProfiles(const ProfileMap& profiles, const std::string& path);

// Exportacao em JSON ("gnm-profiles" versao 1) para consumo por plugin/hook externo.
std::string profilesToJson(const ProfileMap& profiles);
bool exportProfilesJson(const ProfileMap& profiles, const std::string& path);
}
