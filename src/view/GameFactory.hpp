#pragma once

#include <logic/Camera.hpp>
#include <logic/EntityFactory.hpp>

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace View {

class EntityView;

/**
 * @brief Concrete factory that creates logic entities and attaches SFML views.
 *
 * Implements @ref Logic::EntityFactory for use by @ref Logic::World. Each
 * created entity gets a matching @ref EntityView registered for rendering.
 */
class GameFactory final : public Logic::EntityFactory {
public:
    /**
     * @brief Constructs a factory bound to the given camera.
     * @param camera Projection shared by all spawned views.
     */
    explicit GameFactory(Logic::Camera& camera);

    std::shared_ptr<Logic::Wall> createWall(unsigned int id, const Logic::Vector2& position,
                                            const Logic::Vector2& halfExtents,
                                            bool indestructible) override;

    std::shared_ptr<Logic::Character> createCharacter(unsigned int id, const Logic::Vector2& position,
                                                      const Logic::Vector2& halfExtents,
                                                      float moveSpeed, bool isPlayer) override;

    std::shared_ptr<Logic::Bomb> createBomb(unsigned int id, const Logic::Vector2& position,
                                            const Logic::Vector2& halfExtents, float fuseDuration,
                                            int blastRadius, unsigned int ownerId) override;

    std::shared_ptr<Logic::PowerUp> createPowerUp(unsigned int id, const Logic::Vector2& position,
                                                  const Logic::Vector2& halfExtents,
                                                  Logic::PowerUpType powerUpType) override;

    /** @brief Destroys all cached views (called when the world resets). */
    void clearViews() override;

    /**
     * @brief Draws every active entity view in creation order.
     * @param target SFML surface to render into.
     */
    void renderAll(sf::RenderTarget& target) const;

private:
    /** @brief Creates and stores the view matching the entity type. */
    void attachView(const std::shared_ptr<Logic::Entity>& model);

    Logic::Camera& m_camera;
    std::vector<std::shared_ptr<EntityView>> m_views;
};

}
