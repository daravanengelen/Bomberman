#pragma once

#include "logic/EntityFactory.hpp"
#include "logic/Observer.hpp"
#include "logic/Bomb.hpp"
#include "logic/Character.hpp"
#include "logic/Entity.hpp"
#include "logic/PowerUp.hpp"
#include "logic/Wall.hpp"
#include "logic/Vector2.hpp"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Logic {

/** @brief High-level match result. */
enum class GameOutcome {
    Playing,    ///< Match in progress.
    PlayerWon,  ///< Player eliminated all bots.
    PlayerLost, ///< Player was killed.
};

/**
 * @brief Central game-logic controller (MVC model/controller).
 *
 * Owns all entities, runs collision detection, bomb/explosion rules, and dispatches
 * @ref GameEvent notifications. Uses @ref EntityFactory when creating entities so
 * views can be attached transparently.
 */
class World {
public:
    World();

    /** @brief Removes all entities and resets match state. */
    void clear();

    /**
     * @brief Procedurally builds a Bomberman-style grid arena.
     * @param gridWidth Number of columns.
     * @param gridHeight Number of rows.
     * @param emptyCellChance Probability [0, 1] that an inner cell stays empty.
     */
    void generateArena(int gridWidth, int gridHeight, float emptyCellChance = 0.12f);

    /**
     * @brief Spawns the player (top-left) and three bots in the other corners.
     * @param moveSpeed Character movement speed in normalized units per second.
     */
    void spawnCharacters(float moveSpeed = 1.2f);

    /** @brief Sets the factory used to create entities (and their views). */
    void setEntityFactory(const std::shared_ptr<EntityFactory>& factory);

    /** @brief Registers an observer that receives world-level @ref GameEvent updates. */
    void attachWorldObserver(const std::shared_ptr<Observer>& observer);

    /** @brief Adds an entity to the world. */
    std::shared_ptr<Entity> addEntity(std::shared_ptr<Entity> entity);

    /** @brief Queues an entity for removal at the end of the current update. */
    void removeEntity(unsigned int id);

    /**
     * @brief Advances simulation by one frame.
     * @param deltaTime Seconds elapsed since the previous update.
     */
    void update(float deltaTime);

    /**
     * @brief Attempts to move a character in a direction.
     * @return `true` if the move succeeded.
     */
    bool tryMoveCharacter(unsigned int characterId, const Vector2& direction, float deltaTime);

    /**
     * @brief Attempts to place a bomb at the character's current tile.
     * @return `true` if a bomb was placed.
     */
    bool tryPlaceBomb(unsigned int characterId);

    /** @return Entity with the given id, or @c nullptr if not found/active. */
    [[nodiscard]] std::shared_ptr<Entity> findEntity(unsigned int id) const;

    /** @return Character with the given id, or @c nullptr if not a living character. */
    [[nodiscard]] std::shared_ptr<Character> findCharacter(unsigned int id) const;

    /** @return All entities currently in the world. */
    [[nodiscard]] const std::vector<std::shared_ptr<Entity>>& getEntities() const noexcept {
        return m_entities;
    }

    /** @return Current match outcome. */
    [[nodiscard]] GameOutcome getOutcome() const noexcept { return m_outcome; }

    /** @return `true` while the match has not ended. */
    [[nodiscard]] bool isPlaying() const noexcept { return m_outcome == GameOutcome::Playing; }

    /** @return Width of one grid cell in normalized world units. */
    [[nodiscard]] float getCellWidth() const noexcept { return m_cellWidth; }

    /** @return Height of one grid cell in normalized world units. */
    [[nodiscard]] float getCellHeight() const noexcept { return m_cellHeight; }

    /** @return Entity id of the human player, or @c 0 if not found. */
    [[nodiscard]] unsigned int getPlayerId() const;

    /** @return Number of bots spawned at the start of the match. */
    [[nodiscard]] int getSpawnedBotCount() const noexcept { return m_spawnedBotCount; }

private:
    std::shared_ptr<Wall> createWall(const Vector2& position, const Vector2& halfExtents,
                                     bool indestructible);
    std::shared_ptr<Character> createCharacter(const Vector2& position,
                                               const Vector2& halfExtents, float moveSpeed,
                                               bool isPlayer);
    std::shared_ptr<Bomb> createBomb(const Vector2& position, const Vector2& halfExtents,
                                     float fuseDuration, int blastRadius, unsigned int ownerId);
    std::shared_ptr<PowerUp> createPowerUp(const Vector2& position, const Vector2& halfExtents,
                                           PowerUpType powerUpType);

    void queueSpawn(std::shared_ptr<Entity> entity);
    void flushPendingChanges();

    [[nodiscard]] unsigned int allocateId();

    [[nodiscard]] bool isPositionBlocked(const AABB& bounds, unsigned int moverId) const;
    [[nodiscard]] bool canPassBomb(unsigned int characterId, unsigned int bombId) const;
    void updateBombExitState();

    void updateBombs(float deltaTime);
    void detonateBomb(const std::shared_ptr<Bomb>& bomb);
    void castExplosionRay(const Vector2& origin, const Vector2& direction, int maxRadius,
                          unsigned int ownerId);

    void handleCharacterDeath(const std::shared_ptr<Character>& character, unsigned int killerId);
    void tryPickupPowerUp(const std::shared_ptr<Character>& character);
    void applyPowerUp(const std::shared_ptr<Character>& character, PowerUpType type);

    [[nodiscard]] int countActiveBombsFor(unsigned int ownerId) const;
    [[nodiscard]] bool hasBombAt(const Vector2& position) const;
    [[nodiscard]] bool isSpawnCell(int gridX, int gridY, int gridWidth, int gridHeight) const;
    [[nodiscard]] Vector2 gridToWorld(int gridX, int gridY) const;
    /** @brief Converts a world position to integer grid coordinates. */
    void worldToGrid(const Vector2& position, int& gridX, int& gridY) const;
    /** @brief Snaps a world position to the center of its grid cell. */
    [[nodiscard]] Vector2 snapToCellCenter(const Vector2& position) const;
    /** @brief Kills every living character whose bounds overlap the blast cell. */
    void killCharactersInBlastCell(const Vector2& cellCenter, unsigned int ownerId);
    [[nodiscard]] Vector2 getCellHalfExtents() const;
    [[nodiscard]] Vector2 getCharacterHalfExtents() const;
    [[nodiscard]] Vector2 getBombHalfExtents() const;
    [[nodiscard]] Vector2 getPowerUpHalfExtents() const;
    void evaluateOutcome();
    void dispatchWorldEvent(GameEventType type, unsigned int entityId = 0,
                            unsigned int sourceId = 0) const;
    void notifyEntityTicks() const;

    unsigned int m_nextEntityId{1};
    int m_spawnedBotCount{0};
    int m_gridWidth{0};
    int m_gridHeight{0};
    float m_cellWidth{0.1f};
    float m_cellHeight{0.1f};
    GameOutcome m_outcome{GameOutcome::Playing};

    std::vector<std::shared_ptr<Entity>> m_entities;
    std::vector<std::shared_ptr<Entity>> m_pendingSpawns;
    std::vector<unsigned int> m_pendingRemovals;
    std::vector<unsigned int> m_detonationQueue;

    std::unique_ptr<std::vector<unsigned int>> m_detonationScratch;
    std::unordered_map<unsigned int, std::unordered_set<unsigned int>> m_exitedBombs;

    std::shared_ptr<EntityFactory> m_entityFactory;
    std::vector<std::weak_ptr<Observer>> m_worldObservers;
};

}
