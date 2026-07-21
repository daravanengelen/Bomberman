#pragma once

#include "State.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace View {

/**
 * @brief Title screen with the top-five high scores and a Play button.
 *
 * Loads persisted scores on entry. The player can start a match with Enter,
 * Space, or a mouse click on the Play button.
 */
class MenuState final : public State {
public:
    /** @brief Loads high scores from disk. */
    void onEnter(Game& game) override;

    /** @brief Handles Play input and window close events. */
    void handleEvent(Game& game, const sf::Event& event) override;

    void update(Game& game, float deltaTime) override;

    /** @brief Draws the title, scoreboard, and Play button. */
    void render(Game& game, sf::RenderTarget& target) override;

private:
    /** @return `true` if the pixel lies inside the Play button rectangle. */
    [[nodiscard]] bool isPlayButtonClicked(const sf::Vector2i& pixel) const;

    sf::FloatRect m_playButton{300.f, 420.f, 200.f, 60.f};
};

}