#pragma once

#include "logic/Events.hpp"

namespace Logic {

/**
 * @brief Observer interface for @ref GameEvent notifications.
 *
 * Implemented by entity views (to refresh rendering) and by @ref Score
 * (to update match statistics).
 */
class Observer {
public:
    virtual ~Observer() = default;

    /**
     * @brief Called when a subscribed subject or the world emits an event.
     * @param event Event type and associated entity identifiers.
     */
    virtual void onGameEvent(const GameEvent& event) = 0;
};

} // namespace Logic
