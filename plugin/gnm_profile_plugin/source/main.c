// GNM Scale profile plugin: le /data/GoldHEN/gnm/profiles.json (exportado pelo app)
// e, se o perfil do titulo em execucao pedir 60/30/20 FPS, ajusta o flip rate
// via sceVideoOutSetFlipRate (hook pela API publica do GoldHEN Plugin SDK, MIT).
// Padrao de plugin baseado no SDK GoldHEN_Plugins_SDK (OSM, jocover, bucanero, OpenOrbis, SiSTRo, ctn123, nik).
// NAO testado em PS4 real.

#include <Common.h>
#include <orbis/VideoOut.h>
#include "gnm_profile_parse.h"

#define attr_module_hidden __attribute__((weak)) __attribute__((visibility("hidden")))
#define attr_public __attribute__((visibility("default")))

#define GNM_PROFILES_PATH "/data/GoldHEN/gnm/profiles.json"

attr_public const char* g_pluginName = "gnm_profile_plugin";
attr_public const char* g_pluginDesc = "Aplica FPS alvo dos perfis do GNM Scale";
attr_public const char* g_pluginAuth = "GNM Scale";
attr_public uint32_t g_pluginVersion = 0x00000100;

static int g_flipRate = -1;

HOOK_INIT(sceVideoOutSetFlipRate);

int sceVideoOutSetFlipRate_hook(int handle, int fliprate) {
    int rate = (g_flipRate >= 0) ? g_flipRate : fliprate;
    return HOOK_CONTINUE(sceVideoOutSetFlipRate, int (*)(int, int), handle, rate);
}

static int load_profile(void) {
    struct proc_info info;
    memset(&info, 0, sizeof(info));
    if (sys_sdk_proc_info(&info) != 0) return -1;
    klog("[gnm] titleid=%s\n", info.titleid);

    static char buf[16384];
    FILE* f = fopen(GNM_PROFILES_PATH, "rb");
    if (!f) { klog("[gnm] sem %s\n", GNM_PROFILES_PATH); return -1; }
    size_t n = fread(buf, 1, sizeof(buf) - 1, f);
    fclose(f);
    buf[n] = 0;

    int fps = gnm_profile_fps(buf, info.titleid);
    klog("[gnm] fps do perfil=%d\n", fps);
    if (fps == 60) return 0;
    if (fps == 30) return 1;
    if (fps == 20) return 2;
    return -1;
}

int attr_public plugin_load(int argc, const char* argv[]) {
    klog("[GoldHEN] <%s\\Ver.0x%08x> plugin_load\n", g_pluginName, g_pluginVersion);
    g_flipRate = load_profile();
    if (g_flipRate >= 0) HOOK(sceVideoOutSetFlipRate);
    return 0;
}

int attr_public plugin_unload(int argc, const char* argv[]) {
    klog("[GoldHEN] <%s> plugin_unload\n", g_pluginName);
    if (g_flipRate >= 0) UNHOOK(sceVideoOutSetFlipRate);
    return 0;
}

int attr_module_hidden module_start(size_t argc, const void* args) { return 0; }
int attr_module_hidden module_stop(size_t argc, const void* args) { return 0; }
