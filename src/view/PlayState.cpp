#include "PlayState.hpp"

#include "Game.hpp"
#include "GameFactory.hpp"
#include "GameOverState.hpp"
#include "MenuState.hpp"
#include "Ui.hpp"

#include <logic/Random.hpp>
#include <logic/Stopwatch.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <sstream>
#include <unordered_map>

namespace View {

namespace {

bool isPositionThreatened(const Logic::World& world, const Logic::Vector2& position) {
    for (const auto& entity : world.getEntities()) {
        if (!entity || !entity->isActive() || entity->getType() != Logic::EntityType::Bomb) {
            continue;
        }

        const auto bomb = std::static_pointer_cast<Logic::Bomb>(entity);
        if (bomb->isExploded()) {
            continue;
        }

        const Logic::Vector2 delta = position - bomb->getPosition();
        const float blastReach = world.getCellWidth() * static_cast<float>(bomb->getBlastRadius() + 1);

        if (delta.lengthSquared() < 0.22f) {
            return true;
        }

        const bool sameRow = std::abs(delta.y) < 0.10f && std::abs(delta.x) <= blastReach;
        const bool sameColumn = std::abs(delta.x) < 0.10f && std::abs(delta.y) <= blastReach;
        if (sameRow || sameColumn) {
            return true;
        }
    }

    return false;
}

bool bombCanHitPlayer(const Logic::World& world, const Logic::Character& bot, const Logic::Character& player) {
    const Logic::Vector2 toPlayer = player.getPosition() - bot.getPosition();
    const float cellSize = world.getCellWidth();
    return (std::abs(toPlayer.x) < 0.25f && std::abs(toPlayer.y) <= cellSize * 2.2f) ||
           (std::abs(toPlayer.y) < 0.25f && std::abs(toPlayer.x) <= cellSize * 2.2f);
}

} // namespace

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
    static std::unordered_map<unsigned int, float> botDecisionTimers;
    static std::unordered_map<unsigned int, Logic::Vector2> botDirections;
    static std::unordered_map<unsigned int, float> botBombCooldowns;

    auto& random = Logic::Random::getInstance();
    const bool botsMayBomb = m_matchTime >= 5.f;
    const float moveStride = 6.0f;

    for (const auto& entity : m_world.getEntities()) {
        if (!entity || !entity->isActive() || entity->getType() != Logic::EntityType::Character) {
            continue;
        }

        const auto character = std::static_pointer_cast<Logic::Character>(entity);
        if (!character->isAlive() || character->isPlayer()) {
            continue;
        }

        float& decisionTimer = botDecisionTimers[character->getId()];
        decisionTimer -= deltaTime;
        if (decisionTimer > 0.f) {
            continue;
        }

        const auto player = m_world.findCharacter(m_playerId);
        const bool currentlySafe = !isPositionThreatened(m_world, character->getPosition());

        std::array<Logic::Vector2, 4> directions = {
            Logic::Vector2{1.f, 0.f},
            Logic::Vector2{-1.f, 0.f},
            Logic::Vector2{0.f, 1.f},
            Logic::Vector2{0.f, -1.f},
        };

        const int bias = static_cast<int>(character->getId()) % 4;
        std::rotate(directions.begin(), directions.begin() + bias, directions.end());

        if (player) {
            const Logic::Vector2 toPlayer = player->getPosition() - character->getPosition();
            if (std::abs(toPlayer.x) > std::abs(toPlayer.y)) {
                directions[0] = {toPlayer.x >= 0.f ? 1.f : -1.f, 0.f};
                directions[1] = {0.f, toPlayer.y >= 0.f ? 1.f : -1.f};
            } else if (toPlayer.lengthSquared() > 0.f) {
                directions[0] = {0.f, toPlayer.y >= 0.f ? 1.f : -1.f};
                directions[1] = {toPlayer.x >= 0.f ? 1.f : -1.f, 0.f};
            }
        }

        Logic::Vector2 bestDirection{};
        float bestScore = std::numeric_limits<float>::infinity();

        for (const auto& direction : directions) {
            if (direction.lengthSquared() == 0.f) {
                continue;
            }

            const Logic::Vector2 target = character->getPosition() + direction * 0.9f;
            const bool safeTarget = !isPositionThreatened(m_world, target);
            if (!safeTarget) {
                continue;
            }

            float score = 0.f;
            if (player) {
                score += (player->getPosition() - target).lengthSquared();
            }

            const bool movingAwayFromBomb = !isPositionThreatened(m_world, target + direction * 0.25f);
            if (!movingAwayFromBomb) {
                score += 1000.f;
            }

            if (score < bestScore) {
                bestScore = score;
                bestDirection = direction;
            }
        }

        bool moved = false;
        if (bestDirection.lengthSquared() > 0.f) {
            moved = m_world.tryMoveCharacter(character->getId(), bestDirection, deltaTime * moveStride);
        }

        if (moved) {
            botDirections[character->getId()] = bestDirection;
        } else {
            botDirections.erase(character->getId());
            decisionTimer = 0.12f + random.range(0.f, 0.17f);
            continue;
        }

        float& bombCooldown = botBombCooldowns[character->getId()];
        bombCooldown -= deltaTime;

        if (botsMayBomb && bombCooldown <= 0.f && currentlySafe && player) {
            const Logic::Vector2 toPlayer = player->getPosition() - character->getPosition();
            const bool aligned = std::abs(toPlayer.x) < 0.25f || std::abs(toPlayer.y) < 0.25f;
            const bool closeEnough = toPlayer.lengthSquared() < 1.8f;
            const bool safeToBomb = !isPositionThreatened(m_world, character->getPosition());
            const bool canHitPlayer = bombCanHitPlayer(m_world, *character, *player);

            if (aligned && closeEnough && safeToBomb && canHitPlayer && m_world.tryPlaceBomb(character->getId())) {
                bombCooldown = 1.8f + random.range(0.f, 1.4f) + static_cast<float>((character->getId() % 3)) * 0.2f;
                botDirections.erase(character->getId());
            }
        }

        decisionTimer = 0.12f + random.range(0.f, 0.25f) + static_cast<float>((character->getId() % 5)) * 0.05f;
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
    updateBots(deltaTime);
    m_world.update(deltaTime);
}

void PlayState::render(Game& game, sf::RenderTarget& target) {
    target.clear(sf::Color(57, 132, 0)); // floor green from Walltiles.png
    game.getFactory()->renderAll(target);

    if (game.hasFont()) {
        std::ostringstream hud;
        hud << "Score: " << game.getScore()->getCurrentScore();

        if (const auto player = m_world.findCharacter(m_playerId)) {
            hud << "   Fire: " << player->getBlastRadius() << "   Bombs: " << player->getMaxBombs();
        }

        const float centerX = static_cast<float>(Game::kWindowWidth) * 0.5f;
        drawCenteredText(target, game.getFont(), hud.str(), 20, sf::Color(160, 32, 240), centerX, 24.f);
        drawCenteredText(target, game.getFont(), "Arrows move | Space bomb | Esc menu", 16, sf::Color(160, 32, 240),
                         centerX, static_cast<float>(Game::kWindowHeight) - 20.f);
    }
}

} // namespace View
