#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace View {

class Game;

/**
 * @brief Base class for menu, play, and game-over screens.
 *
 * Each concrete state implements the same lifecycle hooks used by @ref Game
 * during the main loop: enter, event handling, update, and render.
 */
class State {
public:
    virtual ~State() = default;

    /** @brief Called once when this state becomes active. */
    virtual void onEnter(Game& game) = 0;

    /**
     * @brief Processes a single SFML input or window event.
     * @param game Application context (window, score, state changes).
     * @param event Polled SFML event.
     */
    virtual void handleEvent(Game& game, const sf::Event& event) = 0;

    /**
     * @brief Advances simulation or idle logic for this screen.
     * @param game Application context.
     * @param deltaTime Seconds since the previous frame.
     */
    virtual void update(Game& game, float deltaTime) = 0;

    /**
     * @brief Draws this screen to the render target.
     * @param game Application context (font, factory, etc.).
     * @param target SFML surface to draw into.
     */
    virtual void render(Game& game, sf::RenderTarget& target) = 0;
};

}
