/**
 * @file GameFactory.cpp
 * @brief Implementation of @ref View::GameFactory.
 */

#include "GameFactory.hpp"

#include "EntityView.hpp"

namespace View {

GameFactory::GameFactory(Logic::Camera& camera) : m_camera(camera) {}

void GameFactory::attachView(const std::shared_ptr<Logic::Entity>& model) {
    if (!model) {
        return;
    }

    std::shared_ptr<EntityView> view;
    switch (model->getType()) {
    case Logic::EntityType::Wall:
        view = std::make_shared<WallView>(model, m_camera);
        break;
    case Logic::EntityType::Character:
        view = std::make_shared<CharacterView>(model, m_camera);
        break;
    case Logic::EntityType::Bomb:
        view = std::make_shared<BombView>(model, m_camera);
        break;
    case Logic::EntityType::PowerUp:
        view = std::make_shared<PowerUpView>(model, m_camera);
        break;
    }

    model->attachObserver(view);
    m_views.push_back(std::move(view));
}

std::shared_ptr<Logic::Wall> GameFactory::createWall(unsigned int id, const Logic::Vector2& position,
                                                     const Logic::Vector2& halfExtents,
                                                     bool indestructible) {
    auto wall = std::make_shared<Logic::Wall>(id, position, halfExtents, indestructible);
    attachView(wall);
    return wall;
}

std::shared_ptr<Logic::Character> GameFactory::createCharacter(
    unsigned int id, const Logic::Vector2& position, const Logic::Vector2& halfExtents,
    float moveSpeed, bool isPlayer) {
    auto character =
        std::make_shared<Logic::Character>(id, position, halfExtents, moveSpeed, isPlayer);
    attachView(character);
    return character;
}

std::shared_ptr<Logic::Bomb> GameFactory::createBomb(unsigned int id, const Logic::Vector2& position,
                                                     const Logic::Vector2& halfExtents,
                                                     float fuseDuration, int blastRadius,
                                                     unsigned int ownerId) {
    auto bomb = std::make_shared<Logic::Bomb>(id, position, halfExtents, fuseDuration, blastRadius,
                                              ownerId);
    attachView(bomb);
    return bomb;
}

std::shared_ptr<Logic::PowerUp> GameFactory::createPowerUp(
    unsigned int id, const Logic::Vector2& position, const Logic::Vector2& halfExtents,
    Logic::PowerUpType powerUpType) {
    auto powerUp = std::make_shared<Logic::PowerUp>(id, position, halfExtents, powerUpType);
    attachView(powerUp);
    return powerUp;
}

void GameFactory::clearViews() {
    m_views.clear();
}

void GameFactory::renderAll(sf::RenderTarget& target) const {
    for (const auto& view : m_views) {
        if (view && view->isVisible()) {
            view->render(target);
        }
    }
}

}
