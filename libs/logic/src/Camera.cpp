/**
 * @file Camera.cpp
 * @brief Implementation of @ref Logic::Camera projection math.
 */

#include "logic/Camera.hpp"

namespace Logic {

Camera::Camera(unsigned int screenWidth, unsigned int screenHeight) {
    setScreenSize(screenWidth, screenHeight);
}

void Camera::setScreenSize(unsigned int screenWidth, unsigned int screenHeight) noexcept {
    m_screenWidth = screenWidth > 0 ? screenWidth : 1;
    m_screenHeight = screenHeight > 0 ? screenHeight : 1;
}

Vector2 Camera::project(const Vector2& world) const {
    const float halfW = static_cast<float>(m_screenWidth) * 0.5f;
    const float halfH = static_cast<float>(m_screenHeight) * 0.5f;
    return {(world.x + 1.f) * halfW, (1.f - world.y) * halfH};
}

Vector2 Camera::unproject(const Vector2& screen) const {
    const float halfW = static_cast<float>(m_screenWidth) * 0.5f;
    const float halfH = static_cast<float>(m_screenHeight) * 0.5f;
    return {(screen.x / halfW) - 1.f, 1.f - (screen.y / halfH)};
}

Vector2 Camera::projectSize(const Vector2& worldSize) const {
    const float halfW = static_cast<float>(m_screenWidth) * 0.5f;
    const float halfH = static_cast<float>(m_screenHeight) * 0.5f;
    return {worldSize.x * halfW, worldSize.y * halfH};
}

}
