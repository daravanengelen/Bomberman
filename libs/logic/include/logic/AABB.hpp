#pragma once

#include "logic/Vector2.hpp"

namespace Logic {

/**
 * @brief Axis-aligned bounding box for 2D collision detection in normalized world space.
 *
 * Stored as min/max corners (minX, minY) and (maxX, maxY).
 */
struct AABB {
    float minX{0.f}; ///< Minimum x-coordinate.
    float minY{0.f}; ///< Minimum y-coordinate.
    float maxX{0.f}; ///< Maximum x-coordinate.
    float maxY{0.f}; ///< Maximum y-coordinate.

    /**
     * @brief Builds an AABB from a center point and half-extents.
     * @param center Center of the box.
     * @param halfExtents Half-width and half-height.
     * @return The corresponding axis-aligned bounding box.
     */
    static AABB fromCenterHalfExtents(const Vector2& center, const Vector2& halfExtents) {
        return {
            center.x - halfExtents.x,
            center.y - halfExtents.y,
            center.x + halfExtents.x,
            center.y + halfExtents.y,
        };
    }

    /**
     * @brief Returns the center point of the box.
     * @return Center as a @ref Vector2.
     */
    [[nodiscard]] Vector2 getCenter() const {
        return {(minX + maxX) * 0.5f, (minY + maxY) * 0.5f};
    }

    /**
     * @brief Returns the half-width and half-height of the box.
     * @return Half-extents as a @ref Vector2.
     */
    [[nodiscard]] Vector2 getHalfExtents() const {
        return {(maxX - minX) * 0.5f, (maxY - minY) * 0.5f};
    }

    /**
     * @brief Tests overlap with another axis-aligned box.
     * @param other The other box to test against.
     * @return `true` if the boxes intersect (edges touching counts as overlap).
     */
    [[nodiscard]] bool intersects(const AABB& other) const noexcept {
        return minX < other.maxX && maxX > other.minX && minY < other.maxY &&
               maxY > other.minY;
    }
};

}
