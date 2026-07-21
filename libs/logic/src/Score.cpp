/**
 * @file Score.cpp
 * @brief Implementation of @ref Logic::Score persistence and event scoring.
 */

#include "logic/Score.hpp"

#include "logic/Stopwatch.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>

namespace Logic {

void Score::resetCurrentScore() noexcept {
    m_currentScore = 0;
}

void Score::onGameEvent(const GameEvent& event) {
    switch (event.type) {
    case GameEventType::WorldTick:
        m_currentScore += static_cast<int>(Stopwatch::getInstance().getDeltaTime() * 10.f);
        break;
    case GameEventType::BlockDestroyed:
        m_currentScore += 50;
        break;
    case GameEventType::PowerUpCollected:
        m_currentScore += 100;
        break;
    case GameEventType::EnemyKilled:
        m_currentScore += 250;
        break;
    case GameEventType::PlayerWon:
        m_currentScore += 1000;
        break;
    case GameEventType::PlayerDied:
        m_currentScore -= 200;
        break;
    default:
        break;
    }
}

void Score::insertHighScore(int score) {
    m_highScores.push_back(score);
    std::sort(m_highScores.begin(), m_highScores.end(), std::greater<int>());
    if (m_highScores.size() > kMaxHighScores) {
        m_highScores.resize(kMaxHighScores);
    }
}

void Score::loadHighScores(const std::string& filePath) {
    m_highScores.clear();
    std::ifstream input(filePath);
    if (!input.is_open()) {
        return;
    }

    int value = 0;
    while (input >> value) {
        insertHighScore(value);
    }
}

void Score::saveHighScores(const std::string& filePath) const {
    std::ofstream output(filePath, std::ios::trunc);
    if (!output.is_open()) {
        throw std::runtime_error("Unable to write high scores to " + filePath);
    }

    for (int score : m_highScores) {
        output << score << '\n';
    }
}

void Score::commitCurrentScore(const std::string& filePath) {
    insertHighScore(m_currentScore);
    saveHighScores(filePath);
}

}
