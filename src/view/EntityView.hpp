#pragma once

#include <logic/Camera.hpp>
#include <logic/Entity.hpp>
#include <logic/Observer.hpp>

#include <SFML/Graphics.hpp>
#include <memory>

namespace View {

/**
 * @brief Base SFML view for a logic @ref Logic::Entity.
 *
 * Converts normalized model coordinates to pixel space via @ref Logic::Camera
 * and redraws when the model dispatches observer events.
 */
class EntityView : public Logic::Observer {
public:
    /**
     * @brief Binds this view to a logic model and camera.
     * @param model Shared entity observed for updates.
     * @param camera Projection used to map world coordinates to pixels.
     */
    EntityView(const std::shared_ptr<Logic::Entity>& model, Logic::Camera& camera);
    ~EntityView() override = default;

    /** @brief Reacts to model/world events and updates visibility or layout. */
    void onGameEvent(const Logic::GameEvent& event) override;

    /** @brief Copies the latest model state into SFML drawables. */
    virtual void syncFromModel() = 0;

    /** @brief Draws the entity to the render target. */
    virtual void render(sf::RenderTarget& target) const = 0;

    /** @return `false` after the model has been removed or deactivated. */
    [[nodiscard]] bool isVisible() const noexcept { return m_visible; }

protected:
    /** @return Pixel position for a normalized world coordinate. */
    [[nodiscard]] sf::Vector2f toPixels(const Logic::Vector2& worldPosition) const;

    /** @return Pixel width and height for normalized world dimensions. */
    [[nodiscard]] sf::Vector2f toPixelSize(const Logic::Vector2& worldSize) const;

    /** @return Locked shared pointer to the observed model, or `nullptr`. */
    [[nodiscard]] std::shared_ptr<Logic::Entity> lockModel() const;

    std::weak_ptr<Logic::Entity> m_model;
    Logic::Camera& m_camera;
    bool m_visible{true};
};

/** @brief Renders walls using tiles from Walltiles.png. */
class WallView final : public EntityView {
public:
    WallView(const std::shared_ptr<Logic::Entity>& model, Logic::Camera& camera);
    void syncFromModel() override;
    void render(sf::RenderTarget& target) const override;

private:
    sf::Sprite m_sprite;
};

/** @brief Renders characters as colored circles. */
class CharacterView final : public EntityView {
public:
    CharacterView(const std::shared_ptr<Logic::Entity>& model, Logic::Camera& camera);
    void syncFromModel() override;
    void render(sf::RenderTarget& target) const override;

private:
    sf::Sprite m_sprite;
};

/** @brief Renders active bombs as small circles. */
class BombView final : public EntityView {
public:
    BombView(const std::shared_ptr<Logic::Entity>& model, Logic::Camera& camera);
    void syncFromModel() override;
    void render(sf::RenderTarget& target) const override;

private:
    sf::Sprite m_sprite;
};

/** @brief Renders power-ups as diamonds (rotated rectangles). */
class PowerUpView final : public EntityView {
public:
    PowerUpView(const std::shared_ptr<Logic::Entity>& model, Logic::Camera& camera);
    void syncFromModel() override;
    void render(sf::RenderTarget& target) const override;

private:
    sf::Sprite m_sprite;
};

}
