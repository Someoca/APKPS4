// Ponto de entrada para o eboot do PS4 (toolchain OpenOrbis). Nao e compilado no build do host.
//
// Correcao da tela preta: o eboot anterior so usava printf e um loop de sleep, sem abrir o
// VideoOut. Sem sceVideoOutOpen + RegisterBuffers + SubmitFlip o PS4 nao apresenta nenhum frame,
// entao o app abria em preto. Agora o app apresenta um framebuffer proprio a cada vsync.
#include "gnm/image.hpp"
#include "gnm/library.hpp"
#include "gnm/pipeline.hpp"
#include <cstdint>
#include <cstdio>
#include <orbis/VideoOut.h>
#include <orbis/libkernel.h>

// Pasta onde o PS4 guarda os jogos instalados (um diretorio por TITLE_ID).
static const char* kGamesRoot = "/user/app";

namespace {
constexpr int kWidth = 1920;
constexpr int kHeight = 1080;
constexpr int kBufferCount = 2;
constexpr size_t kAlign = 0x200000;
constexpr size_t kBufferBytes = static_cast<size_t>(kWidth) * kHeight * 4;

inline uint32_t argb(uint8_t r, uint8_t g, uint8_t b) {
    return 0xFF000000u | (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) | b;
}

struct Display {
    int video = -1;
    OrbisKernelEqueue flipQueue = nullptr;
    uint32_t* buffers[kBufferCount] = {};
    int active = 0;

    bool init() {
        video = sceVideoOutOpen(ORBIS_VIDEO_USER_MAIN, ORBIS_VIDEO_OUT_BUS_MAIN, 0, nullptr);
        if (video < 0) return false;
        if (sceKernelCreateEqueue(&flipQueue, "gnm flip queue") < 0) return false;
        sceVideoOutAddFlipEvent(flipQueue, video, nullptr);

        const size_t total = (kBufferBytes * kBufferCount + kAlign - 1) / kAlign * kAlign;
        off_t phys = 0;
        if (sceKernelAllocateDirectMemory(0, sceKernelGetDirectMemorySize(), total, kAlign, 3, &phys) < 0)
            return false;
        void* mem = nullptr;
        if (sceKernelMapDirectMemory(&mem, total, 0x33, 0, phys, kAlign) < 0) return false;
        for (int i = 0; i < kBufferCount; ++i)
            buffers[i] = reinterpret_cast<uint32_t*>(static_cast<char*>(mem) + kBufferBytes * i);

        OrbisVideoOutBufferAttribute attr;
        sceVideoOutSetBufferAttribute(&attr, 0x80000000, 1, 0, kWidth, kHeight, kWidth);
        if (sceVideoOutRegisterBuffers(video, 0, reinterpret_cast<void**>(buffers), kBufferCount, &attr) != 0)
            return false;
        sceVideoOutSetFlipRate(video, 0);
        return true;
    }

    uint32_t* frame() { return buffers[active]; }

    void present(int frameId) {
        sceVideoOutSubmitFlip(video, active, ORBIS_VIDEO_OUT_FLIP_VSYNC, frameId);
        for (;;) {
            OrbisVideoOutFlipStatus status;
            sceVideoOutGetFlipStatus(video, &status);
            if (status.flipArg == frameId) break;
            OrbisKernelEvent ev;
            int count = 0;
            if (sceKernelWaitEqueue(flipQueue, &ev, 1, &count, nullptr) != 0) break;
        }
        active = (active + 1) % kBufferCount;
    }
};

void fillRect(uint32_t* fb, int x, int y, int w, int h, uint32_t color) {
    for (int j = y; j < y + h && j < kHeight; ++j)
        for (int i = x; i < x + w && i < kWidth; ++i) fb[j * kWidth + i] = color;
}

// Desenha a tela: cabecalho, uma barra por jogo encontrado e o resultado do pipeline.
void drawScene(uint32_t* fb, size_t gameCount, const gnm::Image& preview, int tick) {
    fillRect(fb, 0, 0, kWidth, kHeight, argb(16, 24, 48));
    fillRect(fb, 0, 0, kWidth, 120, argb(0, 90, 160));
    // Indicador de vida: bloco que anda, prova que o loop de apresentacao roda.
    fillRect(fb, 40 + (tick * 4) % (kWidth - 120), 40, 40, 40, argb(255, 255, 255));

    const size_t shown = gameCount < 12 ? gameCount : 12;
    for (size_t i = 0; i < shown; ++i)
        fillRect(fb, 80, 180 + static_cast<int>(i) * 60, 700, 40, argb(60, 160, 90));
    if (gameCount == 0) fillRect(fb, 80, 180, 700, 40, argb(160, 60, 60));

    const int scale = 2;
    const int ox = 900, oy = 180;
    for (uint32_t y = 0; y < preview.height(); ++y) {
        for (uint32_t x = 0; x < preview.width(); ++x) {
            const uint8_t* p = preview.pixel(x, y);
            fillRect(fb, ox + static_cast<int>(x) * scale, oy + static_cast<int>(y) * scale, scale, scale,
                     argb(p[0], p[1], p[2]));
        }
    }
}
}  // namespace

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);

    Display display;
    if (!display.init()) {
        std::printf("GNM SCALE: falha ao iniciar VideoOut\n");
        for (;;) sceKernelUsleep(1000000);
    }

    // Primeiro frame imediato: evita tela preta enquanto a biblioteca e lida.
    fillRect(display.frame(), 0, 0, kWidth, kHeight, argb(0, 90, 160));
    display.present(0);

    const auto games = gnm::scanGames(kGamesRoot);
    std::printf("GNM SCALE PS4 - biblioteca (%u jogos):\n", static_cast<unsigned>(games.size()));
    for (size_t i = 0; i < games.size(); ++i)
        std::printf("  [%u] %s\n", static_cast<unsigned>(i), games[i].name.c_str());

    gnm::Image input(160, 90);
    for (uint32_t y = 0; y < input.height(); ++y) {
        for (uint32_t x = 0; x < input.width(); ++x) {
            uint8_t* p = input.pixel(x, y);
            p[0] = static_cast<uint8_t>(x * 255 / 160);
            p[1] = static_cast<uint8_t>(y * 255 / 90);
            p[2] = 128;
            p[3] = 255;
        }
    }
    auto preview = gnm::upscaleBilinear(input, 320, 180);
    gnm::sharpen(preview, 0.35f);

    for (int frameId = 1;; ++frameId) {
        drawScene(display.frame(), games.size(), preview, frameId);
        display.present(frameId);
    }
}
