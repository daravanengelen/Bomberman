#pragma once

namespace Logic {

/**
 * @brief Axis-aligned box defined by a minimum corner and positive extents.
 *
 * Alternative representation to @ref AABB (min/max corners).
 */
struct BoundingBox {
    float x{0.f};      ///< Minimum x-coordinate (left edge).
    float y{0.f};      ///< Minimum y-coordinate (bottom edge).
    float width{0.f};  ///< Width extending in +x.
    float height{0.f}; ///< Height extending in +y.
};

/**
 * @brief Tests whether two bounding boxes overlap.
 * @param a First box.
 * @param b Second box.
 * @return `true` if the boxes intersect; edge contact counts as overlap.
 */
[[nodiscard]] inline bool checkCollision(const BoundingBox& a, const BoundingBox& b) noexcept {
    return a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height &&
           a.y + a.height > b.y;
}

}
