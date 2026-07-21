#pragma once

#include "State.hpp"

#include <logic/Camera.hpp>
#include <logic/Score.hpp>

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

namespace View {

class GameFactory;

/**
 * @brief SFML application entry point: window, main loop, and screen flow.
 *
 * Owns shared resources (camera, factory, score, font) and delegates input
 * and rendering to the active @ref State.
 */
class Game {
public:
    static constexpr unsigned int kWindowWidth = 800;  ///< Window width in pixels.
    static constexpr unsigned int kWindowHeight = 600; ///< Window height in pixels.
    static constexpr const char* kHighScoreFile = "scores.txt"; ///< Persisted leaderboard path.

    /** @brief Creates the window and shared game services. */
    Game();

    /** @brief Runs the main loop until the window is closed. */
    void run();

    /** @return Main SFML render window. */
    [[nodiscard]] sf::RenderWindow& getWindow() noexcept { return m_window; }

    /** @return Camera that maps normalized logic coordinates to pixels. */
    [[nodiscard]] Logic::Camera& getCamera() noexcept { return m_camera; }

    /** @return Factory used by @ref Logic::World to spawn entities and views. */
    [[nodiscard]] std::shared_ptr<GameFactory> getFactory() const { return m_factory; }

    /** @return Score tracker and high-score store. */
    [[nodiscard]] std::shared_ptr<Logic::Score> getScore() const { return m_score; }

    /** @return UI font when a system font could be loaded. */
    [[nodiscard]] const sf::Font& getFont() const { return m_font; }

    /** @return `true` if @ref getFont returns a usable font. */
    [[nodiscard]] bool hasFont() const noexcept { return m_hasFont; }

    /**
     * @brief Switches to a new screen and calls its @ref State::onEnter hook.
     * @param state Next state; ownership is transferred to @ref Game.
     */
    void changeState(std::unique_ptr<State> state);

private:
    /** @return `true` if a fallback font file was found on disk. */
    bool loadFont();

    sf::RenderWindow m_window;
    Logic::Camera m_camera;
    sf::Font m_font;
    bool m_hasFont{false};

    std::shared_ptr<GameFactory> m_factory;
    std::shared_ptr<Logic::Score> m_score;
    std::unique_ptr<State> m_currentState;
};

}
