/**
 * @file Bomb.cpp
 * @brief Implementation of @ref Logic::Bomb.
 */

#include "logic/Bomb.hpp"

namespace Logic {

Bomb::Bomb(unsigned int id, const Vector2& position, const Vector2& halfExtents,
           float fuseDuration, int blastRadius, unsigned int ownerId)
    : Entity(EntityType::Bomb, id, position, halfExtents),
      m_fuseDuration(fuseDuration),
      m_remainingFuse(fuseDuration),
      m_blastRadius(blastRadius),
      m_ownerId(ownerId) {}

}
