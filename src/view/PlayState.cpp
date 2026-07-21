#include "PlayState.hpp"

#include "Game.hpp"
#include "GameFactory.hpp"
#include "GameOverState.hpp"
#include "MenuState.hpp"
#include "Ui.hpp"

#include <logic/Random.hpp>
#include <logic/Stopwatch.hpp>

#include <sstream>

namespace View {

void PlayState::onEnter(Game& game) {
    game.getScore()->resetCurrentScore();
    m_moveDirection = {};
    m_moveLeft = false;
    m_moveRight = false;
    m_moveUp = false;
    m_moveDown = false;
    m_botDecisionTimer = 1.5f;
    m_matchTime = 0.f;
    Logic::Stopwatch::getInstance().reset();

    m_world.clear();
    m_world.setEntityFactory(game.getFactory());
    m_world.attachWorldObserver(game.getScore());
    m_world.generateArena(13, 11, 0.12f);
    m_world.spawnCharacters();
    m_playerId = m_world.getPlayerId();
}

void PlayState::handleEvent(Game& game, const sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        game.getWindow().close();
        return;
    }

    if (event.type == sf::Event::LostFocus) {
        m_moveLeft = false;
        m_moveRight = false;
        m_moveUp = false;
        m_moveDown = false;
        return;
    }

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        game.changeState(std::make_unique<MenuState>());
        return;
    }

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
        m_world.tryPlaceBomb(m_playerId);
        return;
    }

    if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
        const bool pressed = event.type == sf::Event::KeyPressed;
        switch (event.key.code) {
        case sf::Keyboard::Left:
            m_moveLeft = pressed;
            break;
        case sf::Keyboard::Right:
            m_moveRight = pressed;
            break;
        case sf::Keyboard::Up:
            m_moveUp = pressed;
            break;
        case sf::Keyboard::Down:
            m_moveDown = pressed;
            break;
        default:
            break;
        }
    }
}

void PlayState::updateMovement(float deltaTime) {
    m_moveDirection = {};

    if (m_moveLeft) {
        m_moveDirection.x -= 1.f;
    }
    if (m_moveRight) {
        m_moveDirection.x += 1.f;
    }
    if (m_moveUp) {
        m_moveDirection.y += 1.f;
    }
    if (m_moveDown) {
        m_moveDirection.y -= 1.f;
    }

    if (m_moveDirection.lengthSquared() > 0.f) {
        m_world.tryMoveCharacter(m_playerId, m_moveDirection, deltaTime);
    }
}

void PlayState::updateBots(float deltaTime) {
    m_botDecisionTimer -= deltaTime;
    auto& random = Logic::Random::getInstance();
    const bool botsMayBomb = m_matchTime >= 5.f;

    for (const auto& entity : m_world.getEntities()) {
        if (!entity || !entity->isActive() || entity->getType() != Logic::EntityType::Character) {
            continue;
        }

        const auto character = std::static_pointer_cast<Logic::Character>(entity);
        if (!character->isAlive() || character->isPlayer()) {
            continue;
        }

        if (m_botDecisionTimer <= 0.f) {
            Logic::Vector2 direction{};
            switch (random.range(0, 3)) {
            case 0:
                direction = {1.f, 0.f};
                break;
            case 1:
                direction = {-1.f, 0.f};
                break;
            case 2:
                direction = {0.f, 1.f};
                break;
            default:
                direction = {0.f, -1.f};
                break;
            }
            m_world.tryMoveCharacter(character->getId(), direction, deltaTime);
            if (botsMayBomb && random.range(0, 100) < 8) {
                m_world.tryPlaceBomb(character->getId());
            }
        }
    }

    if (m_botDecisionTimer <= 0.f) {
        m_botDecisionTimer = 0.5f;
    }
}

void PlayState::update(Game& game, float deltaTime) {
    m_matchTime += deltaTime;

    if (!m_world.isPlaying()) {
        try {
            game.getScore()->commitCurrentScore(Game::kHighScoreFile);
        } catch (...) {
        }
        game.changeState(std::make_unique<GameOverState>(m_world.getOutcome()));
        return;
    }

    updateMovement(deltaTime);
    // TODO: Change this to a more sophisticated bot AI later.
    // updateBots(deltaTime);
    m_world.update(deltaTime);
}

void PlayState::render(Game& game, sf::RenderTarget& target) {
    target.clear(sf::Color(57, 132, 0)); // floor green from Walltiles.png
    game.getFactory()->renderAll(target);

    if (game.hasFont()) {
        std::ostringstream hud;
        hud << "Score: " << game.getScore()->getCurrentScore();

        if (const auto player = m_world.findCharacter(m_playerId)) {
            hud << "   Fire: " << player->getBlastRadius()
                << "   Bombs: " << player->getMaxBombs();
        }

        const float centerX = static_cast<float>(Game::kWindowWidth) * 0.5f;
        drawCenteredText(target, game.getFont(), hud.str(), 20, sf::Color(160,32,240), centerX, 24.f);
        drawCenteredText(target, game.getFont(), "Arrows move | Space bomb | Esc menu", 16,
                         sf::Color(160, 32, 240), centerX,
                         static_cast<float>(Game::kWindowHeight) - 20.f);
    }
}

}
