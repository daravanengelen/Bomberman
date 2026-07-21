/**
 * @file Stopwatch.cpp
 * @brief Implementation of @ref Logic::Stopwatch.
 */

#include "logic/Stopwatch.hpp"

namespace Logic {

Stopwatch& Stopwatch::getInstance() {
    static Stopwatch instance;
    return instance;
}

Stopwatch::Stopwatch() {
    reset();
}

void Stopwatch::reset() {
    const TimePoint now = Clock::now();
    m_start = now;
    m_lastFrame = now;
    m_deltaTime = 0.f;
}

void Stopwatch::tick() {
    const TimePoint now = Clock::now();
    m_deltaTime = Seconds{now - m_lastFrame}.count();
    m_lastFrame = now;
}

float Stopwatch::getDeltaTime() const noexcept {
    return m_deltaTime;
}

float Stopwatch::getElapsedTime() const {
    return Seconds{Clock::now() - m_start}.count();
}

}
