#pragma once

#include "logic/Events.hpp"
#include "logic/Observer.hpp"

#include <string>
#include <vector>

namespace Logic {

/**
 * @brief Tracks the current score and persists the top five high scores.
 */
class Score final : public Observer {
public:
    static constexpr std::size_t kMaxHighScores = 5; ///< Number of high scores kept on disk.

    /** @brief Resets the in-progress match score to zero. */
    void resetCurrentScore() noexcept;

    /** @brief Updates score based on the event type. */
    void onGameEvent(const GameEvent& event) override;

    /** @return Score accumulated during the current match. */
    [[nodiscard]] int getCurrentScore() const noexcept { return m_currentScore; }

    /** @return Sorted list of persisted high scores (highest first). */
    [[nodiscard]] const std::vector<int>& getHighScores() const noexcept { return m_highScores; }

    /**
     * @brief Loads high scores from a text file (one score per line).
     * @param filePath Path to the scores file.
     */
    void loadHighScores(const std::string& filePath);

    /**
     * @brief Writes high scores to a text file.
     * @param filePath Path to the scores file.
     * @throws std::runtime_error if the file cannot be written.
     */
    void saveHighScores(const std::string& filePath) const;

    /**
     * @brief Inserts the current score into the leaderboard and saves to disk.
     * @param filePath Path to the scores file.
     */
    void commitCurrentScore(const std::string& filePath);

private:
    /** @brief Inserts a score into the in-memory leaderboard (keeps top @ref kMaxHighScores). */
    void insertHighScore(int score);

    int m_currentScore{0};
    std::vector<int> m_highScores;
};

}
