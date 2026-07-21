/**
 * @file Game.cpp
 * @brief Implementation of @ref View::Game.
 */

#include "Game.hpp"

#include "GameFactory.hpp"
#include "MenuState.hpp"

#include <logic/Stopwatch.hpp>

#include <algorithm>
#include <filesystem>

namespace View {

Game::Game()
    : m_window(sf::VideoMode(kWindowWidth, kWindowHeight), "Bomberman", sf::Style::Close),
      m_camera(kWindowWidth, kWindowHeight),
      m_factory(std::make_shared<GameFactory>(m_camera)),
      m_score(std::make_shared<Logic::Score>()) {
    m_hasFont = loadFont();
    m_window.setFramerateLimit(60);
}

bool Game::loadFont() {
    const char* candidates[] = {
        "/System/Library/Fonts/Supplemental/Comic Sans MS.ttf",
        "/Library/Fonts/Comic Sans MS.ttf",
        "assets/fonts/Comic Sans MS.ttf",
        "assets/fonts/ComicSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/Library/Fonts/Arial.ttf",
    };

    for (const char* path : candidates) {
        if (std::filesystem::exists(path) && m_font.loadFromFile(path)) {
            return true;
        }
    }
    return false;
}

void Game::changeState(std::unique_ptr<State> state) {
    m_currentState = std::move(state);
    if (m_currentState) {
        m_currentState->onEnter(*this);
    }
}

void Game::run() {
    auto& stopwatch = Logic::Stopwatch::getInstance();
    stopwatch.reset();
    changeState(std::make_unique<MenuState>());

    while (m_window.isOpen()) {
        stopwatch.tick();
        const float deltaTime =
            std::min(stopwatch.getDeltaTime(), 1.f / 30.f);

        sf::Event event{};
        while (m_window.pollEvent(event)) {
            if (m_currentState) {
                m_currentState->handleEvent(*this, event);
            }
        }

        if (m_currentState) {
            m_currentState->update(*this, deltaTime);
            m_currentState->render(*this, m_window);
        }

        m_window.display();
    }
}

}
