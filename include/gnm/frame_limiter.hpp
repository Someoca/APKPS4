#pragma once
#include <chrono>
#include <cstdint>
#include <thread>

namespace gnm {
// Mantem um passo fixo por frame (60 FPS por padrao). gameSpeed 1.0 = velocidade original.
class FrameLimiter {
public:
    explicit FrameLimiter(uint32_t targetFps = 60, float gameSpeed = 1.0f)
        : step_(std::chrono::duration_cast<std::chrono::steady_clock::duration>(
              std::chrono::duration<double>(1.0 / (targetFps ? targetFps : 60)))),
          gameSpeed_(gameSpeed),
          next_(std::chrono::steady_clock::now() + step_) {}

    // Passo de simulacao em segundos: constante, escalado pela velocidade do jogo.
    double deltaSeconds() const {
        return std::chrono::duration<double>(step_).count() * gameSpeed_;
    }

    void wait() {
        std::this_thread::sleep_until(next_);
        next_ += step_;
        const auto now = std::chrono::steady_clock::now();
        if (next_ < now) next_ = now + step_;
    }

private:
    std::chrono::steady_clock::duration step_;
    float gameSpeed_;
    std::chrono::steady_clock::time_point next_;
};
}
