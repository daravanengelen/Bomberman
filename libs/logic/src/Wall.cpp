/**
 * @file Wall.cpp
 * @brief Implementation of @ref Logic::Wall.
 */

#include "logic/Wall.hpp"

namespace Logic {

Wall::Wall(unsigned int id, const Vector2& position, const Vector2& halfExtents,
           bool indestructible)
    : Entity(EntityType::Wall, id, position, halfExtents),
      m_indestructible(indestructible) {}

}
