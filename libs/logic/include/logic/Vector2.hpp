#pragma once

#include <cmath>

namespace Logic {

/**
 * @brief Two-dimensional vector used for normalized world coordinates and math.
 */
struct Vector2 {
    float x{0.f}; ///< X component.
    float y{0.f}; ///< Y component.

    Vector2() = default;

    /** @brief Constructs a vector from component values. */
    constexpr Vector2(float xValue, float yValue) : x(xValue), y(yValue) {}

    /** @brief Component-wise addition. */
    Vector2 operator+(const Vector2& other) const {
        return {x + other.x, y + other.y};
    }

    /** @brief Component-wise subtraction. */
    Vector2 operator-(const Vector2& other) const {
        return {x - other.x, y - other.y};
    }

    /** @brief Scalar multiplication. */
    Vector2 operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }

    /** @brief In-place component-wise addition. */
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    /** @return Squared Euclidean length. */
    [[nodiscard]] float lengthSquared() const noexcept {
        return x * x + y * y;
    }

    /**
     * @brief Returns a unit vector in the same direction.
     * @return Normalized vector, or zero vector if length is zero.
     */
    [[nodiscard]] Vector2 normalized() const noexcept {
        const float lengthSquaredValue = lengthSquared();
        if (lengthSquaredValue <= 0.f) {
            return {};
        }
        const float inverseLength = 1.f / std::sqrt(lengthSquaredValue);
        return {x * inverseLength, y * inverseLength};
    }
};

/** @brief Scalar multiplication with the vector on the right. */
inline Vector2 operator*(float scalar, const Vector2& vector) {
    return vector * scalar;
}

}
