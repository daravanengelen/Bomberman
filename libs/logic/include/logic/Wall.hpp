#pragma once

#include "logic/Entity.hpp"

namespace Logic {

/**
 * @brief Arena wall tile (destructible or indestructible).
 */
class Wall : public Entity {
public:
    /**
     * @brief Constructs a wall segment.
     * @param id Unique entity identifier.
     * @param position Normalized world position.
     * @param halfExtents Half-width and half-height of the tile.
     * @param indestructible Whether explosions can destroy this wall.
     */
    Wall(unsigned int id, const Vector2& position, const Vector2& halfExtents,
         bool indestructible);

    /** @return `true` if the wall cannot be destroyed by explosions. */
    [[nodiscard]] bool isIndestructible() const noexcept { return m_indestructible; }

    /** @brief Sets whether the wall is indestructible. */
    void setIndestructible(bool indestructible) noexcept { m_indestructible = indestructible; }

private:
    bool m_indestructible;
};

}
