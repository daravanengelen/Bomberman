#pragma once

#include "logic/Entity.hpp"

namespace Logic {

/**
 * @brief Player or bot character entity.
 */
class Character : public Entity {
public:
    /**
     * @brief Constructs a character at the given spawn position.
     * @param id Unique entity identifier.
     * @param position Normalized world position.
     * @param halfExtents Half-width and half-height of the character bounds.
     * @param moveSpeed Movement speed in normalized units per second.
     * @param isPlayer `true` for the human player, `false` for a bot.
     */
    Character(unsigned int id, const Vector2& position, const Vector2& halfExtents,
              float moveSpeed, bool isPlayer);

    /** @return Movement speed in normalized units per second. */
    [[nodiscard]] float getMoveSpeed() const noexcept { return m_moveSpeed; }

    /** @brief Sets movement speed in normalized units per second. */
    void setMoveSpeed(float moveSpeed) noexcept { m_moveSpeed = moveSpeed; }

    /** @return `true` if this character is controlled by the player. */
    [[nodiscard]] bool isPlayer() const noexcept { return m_isPlayer; }

    /** @return `true` if the character is still alive. */
    [[nodiscard]] bool isAlive() const noexcept { return m_alive; }

    /** @brief Sets whether the character is alive. */
    void setAlive(bool alive) noexcept { m_alive = alive; }

    /** @return Maximum number of bombs that can be active at once. */
    [[nodiscard]] int getMaxBombs() const noexcept { return m_maxBombs; }

    /** @brief Sets the maximum number of simultaneous active bombs. */
    void setMaxBombs(int maxBombs) noexcept { m_maxBombs = maxBombs; }

    /** @return Explosion radius in tiles for each cardinal direction. */
    [[nodiscard]] int getBlastRadius() const noexcept { return m_blastRadius; }

    /** @brief Sets explosion radius in tiles for each cardinal direction. */
    void setBlastRadius(int blastRadius) noexcept { m_blastRadius = blastRadius; }

private:
    float m_moveSpeed;
    bool m_isPlayer;
    bool m_alive{true};
    int m_maxBombs{1};
    int m_blastRadius{1};
};

}
