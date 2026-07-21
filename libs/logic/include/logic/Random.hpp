#pragma once

#include <memory>
#include <random>

namespace Logic {

/**
 * @brief Singleton pseudo-random number generator.
 */
class Random {
public:
    /** @return The single global random generator instance. */
    static Random& getInstance();

    /** @brief Seeds the engine with an explicit value (useful for reproducible tests). */
    void seed(unsigned int value);

    /** @brief Seeds the engine from @c std::random_device. */
    void seedFromRandomDevice();

    /**
     * @brief Returns a uniform integer in [min, max].
     * @param min Inclusive lower bound.
     * @param max Inclusive upper bound.
     */
    [[nodiscard]] int range(int min, int max);

    /**
     * @brief Returns a uniform float in [min, max).
     * @param min Inclusive lower bound.
     * @param max Exclusive upper bound.
     */
    [[nodiscard]] float range(float min, float max);

    /** @return Reference to the underlying Mersenne Twister engine. */
    [[nodiscard]] std::mt19937& engine() noexcept;

    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;
    Random(Random&&) = delete;
    Random& operator=(Random&&) = delete;

private:
    Random();

    std::unique_ptr<std::mt19937> m_engine; ///< Owned PRNG engine.
};

}
