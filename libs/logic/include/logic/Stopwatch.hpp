#pragma once

#include <chrono>

namespace Logic {

/**
 * @brief Singleton frame timer.
 *
 * Provides per-frame @c deltaTime for frame-rate-independent movement and animation.
 */
class Stopwatch {
public:
    /** @return The single global stopwatch instance. */
    static Stopwatch& getInstance();

    /** @brief Resets timing; the next @ref tick() yields @c deltaTime == 0. */
    void reset();

    /** @brief Call once per frame before updating game logic. */
    void tick();

    /** @return Seconds elapsed since the previous @ref tick(). */
    [[nodiscard]] float getDeltaTime() const noexcept;

    /** @return Total seconds elapsed since the last @ref reset(). */
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

    TimePoint m_start{};      ///< Time at last @ref reset().
    TimePoint m_lastFrame{};  ///< Time at previous @ref tick().
    float m_deltaTime{0.f};   ///< Seconds since previous @ref tick().
};

}
