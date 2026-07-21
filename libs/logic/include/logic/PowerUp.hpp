#pragma once

#include "logic/Entity.hpp"

namespace Logic {

/** @brief Types of power-up pickups in the arena. */
enum class PowerUpType {
    Fire,      ///< Increases bomb blast radius.
    ExtraBomb, ///< Allows placing an additional bomb at once.
    Skates,    ///< Increases movement speed.
};

/**
 * @brief Collectible power-up entity.
 */
class PowerUp : public Entity {
public:
    /**
     * @brief Constructs a power-up at the given position.
     * @param id Unique entity identifier.
     * @param position Normalized world position.
     * @param halfExtents Half-width and half-height of the pickup bounds.
     * @param powerUpType Effect applied when collected.
     */
    PowerUp(unsigned int id, const Vector2& position, const Vector2& halfExtents,
            PowerUpType powerUpType);

    /** @return The effect type of this pickup. */
    [[nodiscard]] PowerUpType getPowerUpType() const noexcept { return m_powerUpType; }

private:
    PowerUpType m_powerUpType;
};

}
