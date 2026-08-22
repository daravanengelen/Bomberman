#include "logic/World.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace {

int g_failures = 0;

void expectTrue(const std::string& name, bool condition) {
    if (condition) {
        return;
    }

    std::cerr << "FAIL: " << name << '\n';
    ++g_failures;
}

void expectEqual(const std::string& name, int actual, int expected) {
    if (actual == expected) {
        return;
    }

    std::cerr << "FAIL: " << name << " (expected " << expected << ", got " << actual << ")\n";
    ++g_failures;
}

void expectGreater(const std::string& name, float actual, float expected) {
    if (actual > expected) {
        return;
    }

    std::cerr << "FAIL: " << name << " (expected > " << expected << ", got " << actual << ")\n";
    ++g_failures;
}

void testArenaSetupAndSpawn() {
    Logic::World world;
    world.generateArena(9, 9, 0.f);
    world.spawnCharacters(1.2f);

    int wallCount = 0;
    int characterCount = 0;
    int aliveBotCount = 0;

    for (const auto& entity : world.getEntities()) {
        if (!entity || !entity->isActive()) {
            continue;
        }

        switch (entity->getType()) {
        case Logic::EntityType::Wall:
            ++wallCount;
            break;
        case Logic::EntityType::Character:
            ++characterCount;
            if (!std::static_pointer_cast<Logic::Character>(entity)->isPlayer()) {
                ++aliveBotCount;
            }
            break;
        default:
            break;
        }
    }

    expectTrue("world has a player", world.getPlayerId() != 0);
    expectEqual("spawned bot count", world.getSpawnedBotCount(), 3);
    expectTrue("arena generated walls", wallCount > 0);
    expectEqual("characters count", characterCount, 4);
    expectEqual("bots alive count", aliveBotCount, 3);
}

void testPlayerMovement() {
    Logic::World world;
    world.generateArena(9, 9, 0.f);
    world.spawnCharacters(1.0f);

    const auto player = world.findCharacter(world.getPlayerId());
    if (!player) {
        std::cerr << "FAIL: player was not found\n";
        ++g_failures;
        return;
    }

    const Logic::Vector2 startPosition = player->getPosition();
    const bool moved = world.tryMoveCharacter(player->getId(), Logic::Vector2{1.f, 0.f}, 0.5f);

    expectTrue("player move succeeds", moved);
    expectGreater("player moved right", player->getPosition().x, startPosition.x);
}

void testBombPlacementLimit() {
    Logic::World world;
    world.generateArena(9, 9, 0.f);
    world.spawnCharacters(1.0f);

    const auto player = world.findCharacter(world.getPlayerId());
    if (!player) {
        std::cerr << "FAIL: player was not found\n";
        ++g_failures;
        return;
    }

    player->setMaxBombs(1);

    const bool firstBombPlaced = world.tryPlaceBomb(player->getId());
    const bool secondBombPlaced = world.tryPlaceBomb(player->getId());

    expectTrue("first bomb can be placed", firstBombPlaced);
    expectTrue("second bomb is rejected when max reached", !secondBombPlaced);
}

}

int main() {
    testArenaSetupAndSpawn();
    testPlayerMovement();
    testBombPlacementLimit();

    if (g_failures > 0) {
        std::cerr << "Total failed checks: " << g_failures << '\n';
        return 1;
    }

    std::cout << "All logic tests passed." << std::endl;
    return 0;
}
