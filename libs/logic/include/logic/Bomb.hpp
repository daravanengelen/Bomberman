#pragma once

#include "logic/Entity.hpp"

namespace Logic {

/**
 * @brief Bomb entity placed by a character.
 *
 * Tracks fuse timing, blast radius, and the owner who placed it.
 */
class Bomb : public Entity {
public:
    /**
     * @brief Constructs a bomb at the given world position.
     * @param id Unique entity identifier.
     * @param position Normalized world position in [-1, 1].
     * @param halfExtents Half-width and half-height of the bomb bounds.
     * @param fuseDuration Seconds until detonation.
     * @param blastRadius Tile reach of the explosion in each direction.
     * @param ownerId Identifier of the character that placed the bomb.
     */
    Bomb(unsigned int id, const Vector2& position, const Vector2& halfExtents,
         float fuseDuration, int blastRadius, unsigned int ownerId);

    /** @return Total fuse duration in seconds. */
    [[nodiscard]] float getFuseDuration() const noexcept { return m_fuseDuration; }

    /** @return Remaining fuse time in seconds. */
    [[nodiscard]] float getRemainingFuse() const noexcept { return m_remainingFuse; }

    /** @brief Sets the remaining fuse time in seconds. */
    void setRemainingFuse(float remainingFuse) noexcept { m_remainingFuse = remainingFuse; }

    /** @return Explosion radius in tiles for each cardinal direction. */
    [[nodiscard]] int getBlastRadius() const noexcept { return m_blastRadius; }

    /** @return Identifier of the character that placed this bomb. */
    [[nodiscard]] unsigned int getOwnerId() const noexcept { return m_ownerId; }

    /** @return `true` if the bomb has already detonated. */
    [[nodiscard]] bool isExploded() const noexcept { return m_exploded; }

    /** @brief Marks whether the bomb has detonated. */
    void setExploded(bool exploded) noexcept { m_exploded = exploded; }

private:
    float m_fuseDuration;
    float m_remainingFuse;
    int m_blastRadius;
    unsigned int m_ownerId;
    bool m_exploded{false};
};

}
