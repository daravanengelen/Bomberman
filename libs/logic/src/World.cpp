/**
 * @file World.cpp
 * @brief Implementation of @ref Logic::World simulation and game rules.
 */

#include "logic/World.hpp"

#include "logic/AABB.hpp"
#include "logic/Random.hpp"

#include <algorithm>
#include <cmath>

namespace Logic {

namespace {

constexpr float kWorldMin = -1.f;
constexpr float kWorldMax = 1.f;
constexpr float kBombFuseDuration = 2.f;
constexpr float kPowerUpDropChance = 0.25f;
constexpr float kCharacterHalfExtentScale = 0.38f;
constexpr float kBombHalfExtentScale = 0.30f;
constexpr float kPowerUpHalfExtentScale = 0.28f;

constexpr Vector2 kDirectionRight{1.f, 0.f};
constexpr Vector2 kDirectionLeft{-1.f, 0.f};
constexpr Vector2 kDirectionUp{0.f, 1.f};
constexpr Vector2 kDirectionDown{0.f, -1.f};

[[nodiscard]] AABB clampedBounds(const Vector2& center, const Vector2& halfExtents) {
    Vector2 clampedCenter = center;
    clampedCenter.x = std::clamp(clampedCenter.x, kWorldMin + halfExtents.x, kWorldMax - halfExtents.x);
    clampedCenter.y = std::clamp(clampedCenter.y, kWorldMin + halfExtents.y, kWorldMax - halfExtents.y);
    return AABB::fromCenterHalfExtents(clampedCenter, halfExtents);
}

}

World::World() : m_detonationScratch(std::make_unique<std::vector<unsigned int>>()) {}

void World::clear() {
    if (m_entityFactory) {
        m_entityFactory->clearViews();
    }
    m_entities.clear();
    m_pendingSpawns.clear();
    m_pendingRemovals.clear();
    m_detonationQueue.clear();
    m_exitedBombs.clear();
    m_nextEntityId = 1;
    m_gridWidth = 0;
    m_gridHeight = 0;
    m_spawnedBotCount = 0;
    m_outcome = GameOutcome::Playing;
}

unsigned int World::allocateId() {
    return m_nextEntityId++;
}

void World::setEntityFactory(const std::shared_ptr<EntityFactory>& factory) {
    m_entityFactory = factory;
}

void World::attachWorldObserver(const std::shared_ptr<Observer>& observer) {
    if (observer) {
        m_worldObservers.push_back(observer);
    }
}

void World::dispatchWorldEvent(const GameEventType type, const unsigned int entityId,
                               const unsigned int sourceId) const {
    const GameEvent event{type, entityId, sourceId};
    for (const auto& weakObserver : m_worldObservers) {
        if (const std::shared_ptr<Observer> observer = weakObserver.lock()) {
            observer->onGameEvent(event);
        }
    }
}

void World::notifyEntityTicks() const {
    for (const auto& entity : m_entities) {
        if (entity && entity->isActive()) {
            entity->notifyTick();
        }
    }
}

unsigned int World::getPlayerId() const {
    for (const auto& entity : m_entities) {
        if (!entity || entity->getType() != EntityType::Character) {
            continue;
        }
        const auto character = std::static_pointer_cast<Character>(entity);
        if (character->isPlayer()) {
            return character->getId();
        }
    }
    return 0;
}

Vector2 World::getCellHalfExtents() const {
    return {m_cellWidth * 0.5f, m_cellHeight * 0.5f};
}

Vector2 World::getCharacterHalfExtents() const {
    const Vector2 cellHalfExtents = getCellHalfExtents();
    return {cellHalfExtents.x * kCharacterHalfExtentScale,
            cellHalfExtents.y * kCharacterHalfExtentScale};
}

Vector2 World::getBombHalfExtents() const {
    const Vector2 cellHalfExtents = getCellHalfExtents();
    return {cellHalfExtents.x * kBombHalfExtentScale, cellHalfExtents.y * kBombHalfExtentScale};
}

Vector2 World::getPowerUpHalfExtents() const {
    const Vector2 cellHalfExtents = getCellHalfExtents();
    return {cellHalfExtents.x * kPowerUpHalfExtentScale,
            cellHalfExtents.y * kPowerUpHalfExtentScale};
}

Vector2 World::gridToWorld(int gridX, int gridY) const {
    return {
        kWorldMin + (static_cast<float>(gridX) + 0.5f) * m_cellWidth,
        kWorldMin + (static_cast<float>(gridY) + 0.5f) * m_cellHeight,
    };
}

bool World::isSpawnCell(int gridX, int gridY, int gridWidth, int gridHeight) const {
    const bool topLeft = gridX <= 3 && gridY >= gridHeight - 4;
    const bool topRight = gridX >= gridWidth - 4 && gridY >= gridHeight - 4;
    const bool bottomLeft = gridX <= 3 && gridY <= 3;
    const bool bottomRight = gridX >= gridWidth - 4 && gridY <= 3;
    return topLeft || topRight || bottomLeft || bottomRight;
}

void World::generateArena(int gridWidth, int gridHeight, float emptyCellChance) {
    clear();

    m_gridWidth = gridWidth;
    m_gridHeight = gridHeight;
    m_cellWidth = 2.f / static_cast<float>(gridWidth);
    m_cellHeight = 2.f / static_cast<float>(gridHeight);

    const Vector2 halfExtents = getCellHalfExtents();
    auto& random = Random::getInstance();

    for (int gridY = 0; gridY < gridHeight; ++gridY) {
        for (int gridX = 0; gridX < gridWidth; ++gridX) {
            const Vector2 center = gridToWorld(gridX, gridY);
            const bool isBorder = gridX == 0 || gridY == 0 || gridX == gridWidth - 1 ||
                                  gridY == gridHeight - 1;
            const bool isPillar = gridX % 2 == 0 && gridY % 2 == 0;

            if (isBorder || isPillar) {
                addEntity(createWall(center, halfExtents, true));
                continue;
            }

            if (isSpawnCell(gridX, gridY, gridWidth, gridHeight)) {
                continue;
            }

            if (random.range(0.f, 1.f) < emptyCellChance) {
                continue;
            }

            addEntity(createWall(center, halfExtents, false));
        }
    }
}

void World::spawnCharacters(float moveSpeed) {
    if (m_gridWidth < 3 || m_gridHeight < 3) {
        return;
    }

    m_spawnedBotCount = 0;
    const Vector2 halfExtents = getCharacterHalfExtents();

    const std::vector<std::pair<Vector2, bool>> spawns = {
        {gridToWorld(1, m_gridHeight - 2), true},
        {gridToWorld(m_gridWidth - 2, m_gridHeight - 2), false},
        {gridToWorld(1, 1), false},
        {gridToWorld(m_gridWidth - 2, 1), false},
    };

    for (const auto& [position, isPlayer] : spawns) {
        addEntity(createCharacter(position, halfExtents, moveSpeed, isPlayer));
        if (!isPlayer) {
            ++m_spawnedBotCount;
        }
    }
}

std::shared_ptr<Wall> World::createWall(const Vector2& position, const Vector2& halfExtents,
                                        bool indestructible) {
    const unsigned int id = allocateId();
    if (m_entityFactory) {
        return m_entityFactory->createWall(id, position, halfExtents, indestructible);
    }
    return std::make_shared<Wall>(id, position, halfExtents, indestructible);
}

std::shared_ptr<Character> World::createCharacter(const Vector2& position,
                                                  const Vector2& halfExtents, float moveSpeed,
                                                  bool isPlayer) {
    const unsigned int id = allocateId();
    if (m_entityFactory) {
        return m_entityFactory->createCharacter(id, position, halfExtents, moveSpeed, isPlayer);
    }
    return std::make_shared<Character>(id, position, halfExtents, moveSpeed, isPlayer);
}

std::shared_ptr<Bomb> World::createBomb(const Vector2& position, const Vector2& halfExtents,
                                        float fuseDuration, int blastRadius,
                                        unsigned int ownerId) {
    const unsigned int id = allocateId();
    if (m_entityFactory) {
        return m_entityFactory->createBomb(id, position, halfExtents, fuseDuration, blastRadius,
                                           ownerId);
    }
    return std::make_shared<Bomb>(id, position, halfExtents, fuseDuration, blastRadius, ownerId);
}

std::shared_ptr<PowerUp> World::createPowerUp(const Vector2& position, const Vector2& halfExtents,
                                              PowerUpType powerUpType) {
    const unsigned int id = allocateId();
    if (m_entityFactory) {
        return m_entityFactory->createPowerUp(id, position, halfExtents, powerUpType);
    }
    return std::make_shared<PowerUp>(id, position, halfExtents, powerUpType);
}

std::shared_ptr<Entity> World::addEntity(std::shared_ptr<Entity> entity) {
    m_entities.push_back(std::move(entity));
    return m_entities.back();
}

void World::queueSpawn(std::shared_ptr<Entity> entity) {
    m_pendingSpawns.push_back(std::move(entity));
}

void World::removeEntity(unsigned int id) {
    m_pendingRemovals.push_back(id);
}

void World::flushPendingChanges() {
    for (const unsigned int id : m_pendingRemovals) {
        m_entities.erase(
            std::remove_if(m_entities.begin(), m_entities.end(),
                           [id](const std::shared_ptr<Entity>& entity) {
                               return entity && entity->getId() == id;
                           }),
            m_entities.end());
    }
    m_pendingRemovals.clear();

    for (auto& entity : m_pendingSpawns) {
        m_entities.push_back(std::move(entity));
    }
    m_pendingSpawns.clear();
}

std::shared_ptr<Entity> World::findEntity(unsigned int id) const {
    for (const auto& entity : m_entities) {
        if (entity && entity->isActive() && entity->getId() == id) {
            return entity;
        }
    }
    return nullptr;
}

std::shared_ptr<Character> World::findCharacter(unsigned int id) const {
    const std::shared_ptr<Entity> entity = findEntity(id);
    if (!entity || entity->getType() != EntityType::Character) {
        return nullptr;
    }
    return std::static_pointer_cast<Character>(entity);
}

bool World::canPassBomb(unsigned int characterId, unsigned int bombId) const {
    const auto exitedIt = m_exitedBombs.find(characterId);
    if (exitedIt == m_exitedBombs.end()) {
        return true;
    }
    return !exitedIt->second.contains(bombId);
}

bool World::isPositionBlocked(const AABB& bounds, unsigned int moverId) const {
    for (const auto& entity : m_entities) {
        if (!entity || !entity->isActive() || entity->getId() == moverId) {
            continue;
        }

        if (!entity->getBounds().intersects(bounds)) {
            continue;
        }

        switch (entity->getType()) {
        case EntityType::Wall:
            return true;
        case EntityType::Bomb: {
            const auto bomb = std::static_pointer_cast<Bomb>(entity);
            if (bomb->isExploded()) {
                break;
            }
            if (!canPassBomb(moverId, bomb->getId())) {
                return true;
            }
            break;
        }
        case EntityType::Character: {
            const auto character = std::static_pointer_cast<Character>(entity);
            if (character->isAlive()) {
                return true;
            }
            break;
        }
        case EntityType::PowerUp:
            break;
        }
    }

    return false;
}

void World::updateBombExitState() {
    for (const auto& entity : m_entities) {
        if (!entity || !entity->isActive() || entity->getType() != EntityType::Character) {
            continue;
        }

        const auto character = std::static_pointer_cast<Character>(entity);
        if (!character->isAlive()) {
            continue;
        }

        for (const auto& other : m_entities) {
            if (!other || !other->isActive() || other->getType() != EntityType::Bomb) {
                continue;
            }

            const auto bomb = std::static_pointer_cast<Bomb>(other);
            if (bomb->isExploded()) {
                continue;
            }

            if (!character->getBounds().intersects(bomb->getBounds())) {
                m_exitedBombs[character->getId()].insert(bomb->getId());
            }
        }
    }
}

bool World::tryMoveCharacter(unsigned int characterId, const Vector2& direction,
                             float deltaTime) {
    if (!isPlaying()) {
        return false;
    }

    const std::shared_ptr<Character> character = findCharacter(characterId);
    if (!character || !character->isAlive()) {
        return false;
    }

    const Vector2 normalizedDirection = direction.normalized();
    if (normalizedDirection.lengthSquared() <= 0.f) {
        return false;
    }

    const Vector2 delta = normalizedDirection * (character->getMoveSpeed() * deltaTime);
    const Vector2 targetPosition = character->getPosition() + delta;
    const AABB targetBounds =
        clampedBounds(targetPosition, character->getHalfExtents());

    if (isPositionBlocked(targetBounds, characterId)) {
        return false;
    }

    character->setPosition(targetBounds.getCenter());
    tryPickupPowerUp(character);
    return true;
}

bool World::hasBombAt(const Vector2& position) const {
    const AABB probe = AABB::fromCenterHalfExtents(position, {0.01f, 0.01f});
    for (const auto& entity : m_entities) {
        if (!entity || !entity->isActive() || entity->getType() != EntityType::Bomb) {
            continue;
        }
        const auto bomb = std::static_pointer_cast<Bomb>(entity);
        if (!bomb->isExploded() && bomb->getBounds().intersects(probe)) {
            return true;
        }
    }
    return false;
}

int World::countActiveBombsFor(unsigned int ownerId) const {
    int count = 0;
    for (const auto& entity : m_entities) {
        if (!entity || !entity->isActive() || entity->getType() != EntityType::Bomb) {
            continue;
        }
        const auto bomb = std::static_pointer_cast<Bomb>(entity);
        if (!bomb->isExploded() && bomb->getOwnerId() == ownerId) {
            ++count;
        }
    }
    return count;
}

bool World::tryPlaceBomb(unsigned int characterId) {
    if (!isPlaying()) {
        return false;
    }

    const std::shared_ptr<Character> character = findCharacter(characterId);
    if (!character || !character->isAlive()) {
        return false;
    }

    if (countActiveBombsFor(characterId) >= character->getMaxBombs()) {
        return false;
    }

    if (hasBombAt(character->getPosition())) {
        return false;
    }

    const std::shared_ptr<Bomb> bomb = createBomb(character->getPosition(), getBombHalfExtents(),
                                                  kBombFuseDuration, character->getBlastRadius(),
                                                  characterId);
    addEntity(bomb);
    return true;
}

void World::tryPickupPowerUp(const std::shared_ptr<Character>& character) {
    for (const auto& entity : m_entities) {
        if (!entity || !entity->isActive() || entity->getType() != EntityType::PowerUp) {
            continue;
        }

        if (!entity->getBounds().intersects(character->getBounds())) {
            continue;
        }

        const auto powerUp = std::static_pointer_cast<PowerUp>(entity);
        applyPowerUp(character, powerUp->getPowerUpType());
        if (character->isPlayer()) {
            dispatchWorldEvent(GameEventType::PowerUpCollected, character->getId());
        }
        entity->setActive(false);
        removeEntity(entity->getId());
        return;
    }
}

void World::applyPowerUp(const std::shared_ptr<Character>& character, PowerUpType type) {
    switch (type) {
    case PowerUpType::Fire:
        character->setBlastRadius(character->getBlastRadius() + 1);
        break;
    case PowerUpType::ExtraBomb:
        character->setMaxBombs(character->getMaxBombs() + 1);
        break;
    case PowerUpType::Skates:
        character->setMoveSpeed(character->getMoveSpeed() * 1.25f);
        break;
    }
}

void World::updateBombs(float deltaTime) {
    for (const auto& entity : m_entities) {
        if (!entity || !entity->isActive() || entity->getType() != EntityType::Bomb) {
            continue;
        }

        const auto bomb = std::static_pointer_cast<Bomb>(entity);
        if (bomb->isExploded()) {
            continue;
        }

        bomb->setRemainingFuse(bomb->getRemainingFuse() - deltaTime);
        if (bomb->getRemainingFuse() <= 0.f) {
            m_detonationQueue.push_back(bomb->getId());
        }
    }
}

void World::castExplosionRay(const Vector2& origin, const Vector2& direction, int maxRadius,
                             unsigned int ownerId) {
    const Vector2 step = {direction.x * m_cellWidth, direction.y * m_cellHeight};

    for (int radius = 1; radius <= maxRadius; ++radius) {
        const Vector2 probeCenter = origin + step * static_cast<float>(radius);
        const AABB probe =
            AABB::fromCenterHalfExtents(probeCenter, getCellHalfExtents());

        bool blocked = false;

        for (const auto& entity : m_entities) {
            if (!entity || !entity->isActive()) {
                continue;
            }

            if (!entity->getBounds().intersects(probe)) {
                continue;
            }

            switch (entity->getType()) {
            case EntityType::Wall: {
                const auto wall = std::static_pointer_cast<Wall>(entity);
                if (wall->isIndestructible()) {
                    return;
                }

                const unsigned int wallId = entity->getId();
                entity->setActive(false);
                removeEntity(wallId);
                dispatchWorldEvent(GameEventType::BlockDestroyed, wallId, ownerId);

                if (Random::getInstance().range(0.f, 1.f) < kPowerUpDropChance) {
                    const int roll = Random::getInstance().range(0, 2);
                    const PowerUpType type = static_cast<PowerUpType>(roll);
                    // Spawn on the destroyed wall's cell center (not probeCenter),
                    // otherwise an off-grid bomb ray can place the pickup on a neighbour.
                    queueSpawn(createPowerUp(entity->getPosition(), getPowerUpHalfExtents(), type));
                }

                blocked = true;
                break;
            }
            case EntityType::Bomb: {
                const auto bomb = std::static_pointer_cast<Bomb>(entity);
                if (!bomb->isExploded()) {
                    m_detonationQueue.push_back(bomb->getId());
                }
                blocked = true;
                break;
            }
            case EntityType::Character: {
                const auto character = std::static_pointer_cast<Character>(entity);
                if (character->isAlive()) {
                    handleCharacterDeath(character, ownerId);
                }
                break;
            }
            case EntityType::PowerUp:
                entity->setActive(false);
                removeEntity(entity->getId());
                break;
            }
        }

        if (blocked) {
            return;
        }
    }
}

void World::detonateBomb(const std::shared_ptr<Bomb>& bomb) {
    if (!bomb || bomb->isExploded()) {
        return;
    }

    bomb->setExploded(true);
    bomb->setActive(false);
    removeEntity(bomb->getId());

    const Vector2 origin = bomb->getPosition();
    const int blastRadius = bomb->getBlastRadius();

    const unsigned int ownerId = bomb->getOwnerId();

    for (const auto& entity : m_entities) {
        if (!entity || !entity->isActive() || entity->getType() != EntityType::Character) {
            continue;
        }

        const auto character = std::static_pointer_cast<Character>(entity);
        if (character->isAlive() && character->getBounds().intersects(bomb->getBounds())) {
            handleCharacterDeath(character, ownerId);
        }
    }

    castExplosionRay(origin, kDirectionRight, blastRadius, ownerId);
    castExplosionRay(origin, kDirectionLeft, blastRadius, ownerId);
    castExplosionRay(origin, kDirectionUp, blastRadius, ownerId);
    castExplosionRay(origin, kDirectionDown, blastRadius, ownerId);
}

void World::handleCharacterDeath(const std::shared_ptr<Character>& character,
                                 unsigned int killerId) {
    character->setAlive(false);
    character->setActive(false);

    if (character->isPlayer()) {
        dispatchWorldEvent(GameEventType::PlayerDied, character->getId());
    } else {
        const std::shared_ptr<Character> killer = findCharacter(killerId);
        if (killer && killer->isPlayer()) {
            dispatchWorldEvent(GameEventType::EnemyKilled, character->getId(), killerId);
        }
    }

    evaluateOutcome();
}

void World::evaluateOutcome() {
    std::shared_ptr<Character> player;
    int aliveBots = 0;

    for (const auto& entity : m_entities) {
        if (!entity || entity->getType() != EntityType::Character) {
            continue;
        }

        const auto character = std::static_pointer_cast<Character>(entity);
        if (character->isPlayer()) {
            player = character;
            continue;
        }

        if (character->isAlive()) {
            ++aliveBots;
        }
    }

    if (!player || !player->isAlive()) {
        m_outcome = GameOutcome::PlayerLost;
        return;
    }

    if (aliveBots == 0 && m_spawnedBotCount > 0) {
        m_outcome = GameOutcome::PlayerWon;
        dispatchWorldEvent(GameEventType::PlayerWon, player->getId());
    }
}

void World::update(float deltaTime) {
    if (!isPlaying()) {
        return;
    }

    updateBombExitState();
    updateBombs(deltaTime);

    m_detonationScratch->assign(m_detonationQueue.begin(), m_detonationQueue.end());
    m_detonationQueue.clear();
    std::sort(m_detonationScratch->begin(), m_detonationScratch->end());
    m_detonationScratch->erase(
        std::unique(m_detonationScratch->begin(), m_detonationScratch->end()),
        m_detonationScratch->end());

    for (const unsigned int bombId : *m_detonationScratch) {
        const std::shared_ptr<Entity> entity = findEntity(bombId);
        if (!entity || entity->getType() != EntityType::Bomb) {
            continue;
        }
        detonateBomb(std::static_pointer_cast<Bomb>(entity));
    }

    flushPendingChanges();
    evaluateOutcome();
    notifyEntityTicks();
    dispatchWorldEvent(GameEventType::WorldTick);
}

}
