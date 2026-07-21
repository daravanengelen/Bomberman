#include "GameOverState.hpp"

#include "Game.hpp"
#include "MenuState.hpp"
#include "Ui.hpp"

#include <sstream>

namespace View {

GameOverState::GameOverState(const Logic::GameOutcome outcome) : m_outcome(outcome) {}

void GameOverState::onEnter(Game& /*game*/) {}

void GameOverState::handleEvent(Game& game, const sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        game.getWindow().close();
        return;
    }

    if (event.type == sf::Event::KeyPressed &&
        (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space)) {
        game.changeState(std::make_unique<MenuState>());
    }
}

void GameOverState::update(Game& /*game*/, float /*deltaTime*/) {}

void GameOverState::render(Game& game, sf::RenderTarget& target) {
    target.clear(sf::Color(18, 18, 28));

    const bool won = m_outcome == Logic::GameOutcome::PlayerWon;
    const sf::Color titleColor = won ? sf::Color(120, 255, 140) : sf::Color(255, 100, 100);
    const float centerX = static_cast<float>(Game::kWindowWidth) * 0.5f;

    if (game.hasFont()) {
        drawCenteredText(target, game.getFont(), won ? "You Win!" : "You Lose!", 44, titleColor,
                         centerX, 220.f);
        std::ostringstream scoreLine;
        scoreLine << "Final Score: " << game.getScore()->getCurrentScore();
        drawCenteredText(target, game.getFont(), scoreLine.str(), 26, sf::Color::White, centerX,
                         300.f);
        drawCenteredText(target, game.getFont(), "Press Enter to return to menu", 20,
                         sf::Color(200, 200, 210), centerX, 420.f);
    } else {
        drawPanel(target, {250.f, 180.f, 300.f, 80.f}, titleColor, sf::Color::Black);
        drawPanel(target, {220.f, 400.f, 360.f, 50.f}, sf::Color(80, 80, 120), sf::Color::White);
    }
}

}
