#pragma once

#include "logic/Vector2.hpp"

namespace Logic {

/**
 * @brief Projects normalized world coordinates to pixel space and back.
 *
 * World space uses [-1, 1] with origin at the center and +Y up.
 * Screen space uses pixels with origin at the top-left and +Y down.
 */
class Camera {
public:
    Camera() = default;

    /**
     * @brief Constructs a camera for the given screen resolution.
     * @param screenWidth Window width in pixels.
     * @param screenHeight Window height in pixels.
     */
    Camera(unsigned int screenWidth, unsigned int screenHeight);

    /**
     * @brief Updates the target screen resolution.
     * @param screenWidth Window width in pixels.
     * @param screenHeight Window height in pixels.
     */
    void setScreenSize(unsigned int screenWidth, unsigned int screenHeight) noexcept;

    /** @return Screen width in pixels. */
    [[nodiscard]] unsigned int getScreenWidth() const noexcept { return m_screenWidth; }

    /** @return Screen height in pixels. */
    [[nodiscard]] unsigned int getScreenHeight() const noexcept { return m_screenHeight; }

    /**
     * @brief Maps a world-space point to pixel coordinates.
     * @param world Point in normalized world space.
     * @return Corresponding pixel position.
     */
    [[nodiscard]] Vector2 project(const Vector2& world) const;

    /**
     * @brief Maps a pixel position back to world space.
     * @param screen Point in pixel coordinates.
     * @return Corresponding normalized world position.
     */
    [[nodiscard]] Vector2 unproject(const Vector2& screen) const;

    /**
     * @brief Maps a world-space size to pixel dimensions (no origin offset).
     * @param worldSize Extents in normalized world units.
     * @return Size in pixels.
     */
    [[nodiscard]] Vector2 projectSize(const Vector2& worldSize) const;

private:
    unsigned int m_screenWidth{1};  ///< Current screen width in pixels.
    unsigned int m_screenHeight{1}; ///< Current screen height in pixels.
};

}
