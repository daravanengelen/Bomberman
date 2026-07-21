#pragma once

#include "logic/Bomb.hpp"
#include "logic/Character.hpp"
#include "logic/PowerUp.hpp"
#include "logic/Vector2.hpp"
#include "logic/Wall.hpp"

#include <memory>

namespace Logic {

/**
 * @brief Abstract factory used by @ref World to spawn entities.
 *
 * The logic layer defines this interface; the view layer provides a concrete
 * implementation that also attaches SFML views to new models.
 */
class EntityFactory {
public:
    virtual ~EntityFactory() = default;

    /**
     * @brief Creates a wall tile at the given grid cell.
     * @param id Unique entity identifier.
     * @param position Normalized world position.
     * @param halfExtents Collision half-extents.
     * @param indestructible `true` for permanent arena blocks.
     * @return Shared pointer to the new wall.
     */
    virtual std::shared_ptr<Wall> createWall(unsigned int id, const Vector2& position,
                                             const Vector2& halfExtents,
                                             bool indestructible) = 0;

    /**
     * @brief Creates a playable or bot character.
     * @param id Unique entity identifier.
     * @param position Spawn position in normalized coordinates.
     * @param halfExtents Collision half-extents.
     * @param moveSpeed Movement speed multiplier for delta-time integration.
     * @param isPlayer `true` for the human-controlled character.
     * @return Shared pointer to the new character.
     */
    virtual std::shared_ptr<Character> createCharacter(unsigned int id, const Vector2& position,
                                                       const Vector2& halfExtents, float moveSpeed,
                                                       bool isPlayer) = 0;

    /**
     * @brief Creates a bomb owned by a character.
     * @param id Unique entity identifier.
     * @param position Placement position.
     * @param halfExtents Collision half-extents.
     * @param fuseDuration Seconds until detonation.
     * @param blastRadius Tile reach of the explosion in each direction.
     * @param ownerId Identifier of the placing character.
     * @return Shared pointer to the new bomb.
     */
    virtual std::shared_ptr<Bomb> createBomb(unsigned int id, const Vector2& position,
                                             const Vector2& halfExtents, float fuseDuration,
                                             int blastRadius, unsigned int ownerId) = 0;

    /**
     * @brief Creates a collectible power-up.
     * @param id Unique entity identifier.
     * @param position World position.
     * @param halfExtents Pickup collision half-extents.
     * @param powerUpType Effect applied when collected.
     * @return Shared pointer to the new power-up.
     */
    virtual std::shared_ptr<PowerUp> createPowerUp(unsigned int id, const Vector2& position,
                                                   const Vector2& halfExtents,
                                                   PowerUpType powerUpType) = 0;

    /** @brief Removes all view objects held by the concrete factory. */
    virtual void clearViews() = 0;
};

} // namespace Logic
