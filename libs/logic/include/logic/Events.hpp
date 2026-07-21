#pragma once

namespace Logic {

/** @brief Categories of events dispatched to @ref Observer implementations. */
enum class GameEventType {
    WorldTick,        ///< Fired once per simulation step.
    EntityMoved,      ///< An entity changed position.
    EntityRemoved,    ///< An entity was removed from the world.
    BlockDestroyed,   ///< A destructible wall was destroyed by an explosion.
    PowerUpCollected, ///< The player picked up a power-up.
    EnemyKilled,      ///< The player eliminated a bot.
    PlayerDied,       ///< The player was killed.
    PlayerWon,        ///< The player is the last character standing.
};

/** @brief Payload sent to observers when game state changes. */
struct GameEvent {
    GameEventType type{GameEventType::WorldTick}; ///< Kind of update.
    unsigned int entityId{0};                     ///< Primary entity involved, if any.
    unsigned int sourceId{0};                     ///< Secondary actor (e.g. bomb owner).
};

} // namespace Logic
