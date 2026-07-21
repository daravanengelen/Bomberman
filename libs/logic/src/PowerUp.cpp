/**
 * @file PowerUp.cpp
 * @brief Implementation of @ref Logic::PowerUp.
 */

#include "logic/PowerUp.hpp"

namespace Logic {

PowerUp::PowerUp(unsigned int id, const Vector2& position, const Vector2& halfExtents,
                 PowerUpType powerUpType)
    : Entity(EntityType::PowerUp, id, position, halfExtents),
      m_powerUpType(powerUpType) {}

}
