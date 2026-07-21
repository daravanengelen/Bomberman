/**
 * @file Character.cpp
 * @brief Implementation of @ref Logic::Character.
 */

#include "logic/Character.hpp"

namespace Logic {

Character::Character(unsigned int id, const Vector2& position, const Vector2& halfExtents,
                     float moveSpeed, bool isPlayer)
    : Entity(EntityType::Character, id, position, halfExtents),
      m_moveSpeed(moveSpeed),
      m_isPlayer(isPlayer) {}

}
