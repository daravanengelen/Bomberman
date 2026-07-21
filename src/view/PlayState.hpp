#pragma once

#include "State.hpp"

#include <logic/Vector2.hpp>
#include <logic/World.hpp>

namespace View {

/**
 * @brief Active gameplay screen: input, simulation, and HUD.
 *
 * Owns a @ref Logic::World instance, forwards player actions to it, runs a
 * simple bot update step, and renders entities through @ref GameFactory.
 */
class PlayState final : public State {
public:
    /** @brief Resets score, builds the arena, and spawns characters. */
    void onEnter(Game& game) override;

    /** @brief Handles movement keys, bomb placement, pause, and window close. */
    void handleEvent(Game& game, const sf::Event& event) override;

    /** @brief Steps simulation and transitions to @ref GameOverState when finished. */
    void update(Game& game, float deltaTime) override;

    /** @brief Clears the frame, draws entities, and overlays the HUD. */
    void render(Game& game, sf::RenderTarget& target) override;

private:
    /** @brief Applies simple random movement and occasional bombing for bots. */
    void updateBots(float deltaTime);

    /** @brief Converts held arrow keys into continuous player movement. */
    void updateMovement(float deltaTime);

    Logic::World m_world;
    unsigned int m_playerId{0};
    Logic::Vector2 m_moveDirection{};
    bool m_moveLeft{false};
    bool m_moveRight{false};
    bool m_moveUp{false};
    bool m_moveDown{false};
    float m_botDecisionTimer{1.5f};
    float m_matchTime{0.f};
};

}
