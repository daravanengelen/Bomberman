/**
 * @file Entity.cpp
 * @brief Implementation of @ref Logic::Entity.
 */

#include "logic/Entity.hpp"

namespace Logic {

Entity::Entity(EntityType type, unsigned int id, const Vector2& position, const Vector2& halfExtents)
    : m_type(type), m_id(id), m_position(position), m_halfExtents(halfExtents) {}

void Entity::setPosition(const Vector2& position) {
    m_position = position;
    notifyMoved();
}

void Entity::setActive(bool active) {
    if (m_active && !active) {
        m_active = active;
        notifyRemoved();
        return;
    }
    m_active = active;
}

void Entity::attachObserver(const std::shared_ptr<Observer>& observer) {
    if (!observer) {
        return;
    }
    m_observers.push_back(observer);
}

void Entity::notifyObservers(const GameEvent& event) const {
    for (const auto& weakObserver : m_observers) {
        if (const std::shared_ptr<Observer> observer = weakObserver.lock()) {
            observer->onGameEvent(event);
        }
    }
}

void Entity::notifyTick() const {
    GameEvent event;
    event.type = GameEventType::WorldTick;
    event.entityId = m_id;
    notifyObservers(event);
}

void Entity::notifyMoved() const {
    GameEvent event;
    event.type = GameEventType::EntityMoved;
    event.entityId = m_id;
    notifyObservers(event);
}

void Entity::notifyRemoved() const {
    GameEvent event;
    event.type = GameEventType::EntityRemoved;
    event.entityId = m_id;
    notifyObservers(event);
}

}
