#pragma once

#include <chrono>

namespace Logic {

/// Monotonic frame timer for the game loop (singleton).
class Stopwatch {
public:
    static Stopwatch& getInstance();

    /// Reset the clock; the next tick() produces deltaTime == 0.
    void reset();

    /// Call once per frame before updating game logic.
    void tick();

    /// Seconds elapsed since the previous tick().
    [[nodiscard]] float getDeltaTime() const noexcept;

    /// Total seconds elapsed since the last reset().
    [[nodiscard]] float getElapsedTime() const;

    Stopwatch(const Stopwatch&) = delete;
    Stopwatch& operator=(const Stopwatch&) = delete;
    Stopwatch(Stopwatch&&) = delete;
    Stopwatch& operator=(Stopwatch&&) = delete;

private:
    Stopwatch();

    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Seconds = std::chrono::duration<float>;

    TimePoint m_start{};
    TimePoint m_lastFrame{};
    float m_deltaTime{0.f};
};

} // namespace Logic
