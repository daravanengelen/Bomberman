#include "logic/Random.hpp"

namespace Logic {

Random& Random::getInstance() {
    static Random instance;
    return instance;
}

Random::Random() : m_engine(std::make_unique<std::mt19937>()) {
    seedFromRandomDevice();
}

void Random::seed(unsigned int value) {
    m_engine->seed(value);
}

void Random::seedFromRandomDevice() {
    std::random_device rd;
    seed(rd());
}

int Random::range(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(*m_engine);
}

float Random::range(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(*m_engine);
}

std::mt19937& Random::engine() noexcept {
    return *m_engine;
}

} // namespace Logic
