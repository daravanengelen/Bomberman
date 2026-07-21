#include "MenuState.hpp"

#include "Game.hpp"
#include "PlayState.hpp"
#include "Ui.hpp"

#include <logic/Score.hpp>

#include <sstream>

namespace View {

void MenuState::onEnter(Game& game) {
    game.getScore()->loadHighScores(Game::kHighScoreFile);
}

bool MenuState::isPlayButtonClicked(const sf::Vector2i& pixel) const {
    return m_playButton.contains(static_cast<float>(pixel.x), static_cast<float>(pixel.y));
}

void MenuState::handleEvent(Game& game, const sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        game.getWindow().close();
        return;
    }

    if (event.type == sf::Event::KeyPressed &&
        (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space)) {
        game.changeState(std::make_unique<PlayState>());
        return;
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (isPlayButtonClicked({event.mouseButton.x, event.mouseButton.y})) {
            game.changeState(std::make_unique<PlayState>());
        }
    }
}

void MenuState::update(Game& /*game*/, float /*deltaTime*/) {}

void MenuState::render(Game& game, sf::RenderTarget& target) {
    target.clear(sf::Color(24, 28, 38));

    const float centerX = static_cast<float>(Game::kWindowWidth) * 0.5f;

    if (game.hasFont()) {
        drawCenteredText(target, game.getFont(), "BOMBERMAN", 48, sf::Color(255, 220, 90), centerX,
                         90.f);
        drawCenteredText(target, game.getFont(), "Top Scores", 28, sf::Color::White, centerX, 170.f);
    } else {
        drawPanel(target, {250.f, 50.f, 300.f, 60.f}, sf::Color(255, 220, 90), sf::Color::Black);
    }

    const auto& highScores = game.getScore()->getHighScores();
    float scoreY = 220.f;
    for (std::size_t index = 0; index < Logic::Score::kMaxHighScores; ++index) {
        std::ostringstream line;
        line << (index + 1) << ". ";
        if (index < highScores.size()) {
            line << highScores[index];
        } else {
            line << "-";
        }

        if (game.hasFont()) {
            drawCenteredText(target, game.getFont(), line.str(), 22, sf::Color(210, 210, 220),
                             centerX, scoreY);
        } else {
            const float width = index < highScores.size()
                                    ? static_cast<float>(highScores[index]) * 0.6f + 40.f
                                    : 40.f;
            drawPanel(target, {centerX - width * 0.5f, scoreY - 12.f, width, 24.f},
                      sf::Color(100, 180, 255), sf::Color::Black);
        }
        scoreY += 34.f;
    }

    drawPanel(target, m_playButton, sf::Color(70, 170, 90), sf::Color(230, 255, 230));
    if (game.hasFont()) {
        drawCenteredText(target, game.getFont(), "PLAY", 30, sf::Color::White, centerX,
                         m_playButton.top + m_playButton.height * 0.5f);
    }
}

}
