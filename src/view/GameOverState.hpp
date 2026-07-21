#pragma once

#include "State.hpp"

#include <logic/World.hpp>

namespace View {

/**
 * @brief End-of-match screen shown after a win or loss.
 *
 * Displays the outcome and waits for the player to return to the menu.
 */
class GameOverState final : public State {
public:
    /**
     * @brief Stores the match result to display.
     * @param outcome Win or loss flag from @ref Logic::World.
     */
    explicit GameOverState(Logic::GameOutcome outcome);

    /** @brief Commits the current score to the high-score file. */
    void onEnter(Game& game) override;

    /** @brief Handles keyboard input to return to the menu or quit. */
    void handleEvent(Game& game, const sf::Event& event) override;

    void update(Game& game, float deltaTime) override;

    /** @brief Draws the outcome message and restart hint. */
    void render(Game& game, sf::RenderTarget& target) override;

private:
    Logic::GameOutcome m_outcome;
};

}
