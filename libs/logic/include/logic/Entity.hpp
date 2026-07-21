#pragma once

#include "logic/AABB.hpp"
#include "logic/Events.hpp"
#include "logic/Observer.hpp"
#include "logic/Vector2.hpp"

#include <memory>
#include <vector>

namespace Logic {

/** @brief Discriminator for concrete entity types stored in @ref World. */
enum class EntityType { Character, Bomb, Wall, PowerUp };

/**
 * @brief Base model for all world entities (MVC model + observer subject).
 *
 * Holds normalized position and axis-aligned bounds. Concrete types such as
 * @ref Character and @ref Wall extend this class. Attached @ref Observer
 * instances (typically views) receive @ref GameEvent updates on change.
 */
class Entity {
public:
    virtual ~Entity() = default;

    /** @return Concrete type tag used for safe down-casting. */
    [[nodiscard]] EntityType getType() const noexcept { return m_type; }

    /** @return Unique identifier assigned by @ref World. */
    [[nodiscard]] unsigned int getId() const noexcept { return m_id; }

    /** @return Center position in normalized world coordinates [-1, 1]. */
    [[nodiscard]] Vector2 getPosition() const noexcept { return m_position; }

    /** @brief Updates the entity center position and notifies observers. */
    void setPosition(const Vector2& position);

    /** @return Half-width and half-height of the entity bounds. */
    [[nodiscard]] Vector2 getHalfExtents() const noexcept { return m_halfExtents; }

    /** @brief Sets the half-extents used for collision and rendering. */
    void setHalfExtents(const Vector2& halfExtents) { m_halfExtents = halfExtents; }

    /** @return Axis-aligned bounding box derived from position and half-extents. */
    [[nodiscard]] AABB getBounds() const {
        return AABB::fromCenterHalfExtents(m_position, m_halfExtents);
    }

    /** @return `true` while the entity is still part of the simulation. */
    [[nodiscard]] bool isActive() const noexcept { return m_active; }

    /** @brief Marks the entity active or inactive without removing it immediately. */
    void setActive(bool active);

    /** @brief Registers an observer that receives entity-level events. */
    void attachObserver(const std::shared_ptr<Observer>& observer);

    /** @brief Dispatches a @ref GameEventType::WorldTick event to observers. */
    void notifyTick() const;

    /** @brief Dispatches a @ref GameEventType::EntityMoved event to observers. */
    void notifyMoved() const;

    /** @brief Dispatches a @ref GameEventType::EntityRemoved event to observers. */
    void notifyRemoved() const;

protected:
    /**
     * @brief Constructs a base entity with type, identity, and bounds.
     * @param type Concrete entity category.
     * @param id Unique identifier.
     * @param position Initial normalized position.
     * @param halfExtents Initial collision half-extents.
     */
    Entity(EntityType type, unsigned int id, const Vector2& position, const Vector2& halfExtents);

    /** @brief Sends a custom event to all registered observers. */
    void notifyObservers(const GameEvent& event) const;

private:
    EntityType m_type;
    unsigned int m_id;
    Vector2 m_position;
    Vector2 m_halfExtents;
    bool m_active{true};
    std::vector<std::weak_ptr<Observer>> m_observers;
};

} // namespace Logic
