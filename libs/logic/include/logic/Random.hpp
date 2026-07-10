#pragma once

#include <memory>
#include <random>

namespace Logic {

/// Thread-unsafe pseudo-random generator (singleton).
class Random {
public:
    static Random& getInstance();

    void seed(unsigned int value);
    void seedFromRandomDevice();

    /// Inclusive integer range [min, max].
    [[nodiscard]] int range(int min, int max);

    /// Uniform float in [min, max).
    [[nodiscard]] float range(float min, float max);

    /// Direct access to the underlying engine (e.g. for custom distributions).
    [[nodiscard]] std::mt19937& engine() noexcept;

    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;
    Random(Random&&) = delete;
    Random& operator=(Random&&) = delete;

private:
    Random();

    std::unique_ptr<std::mt19937> m_engine;
};

} // namespace Logic
