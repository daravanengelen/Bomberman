/**
 * @file EntityView.cpp
 * @brief SFML views for logic entities.
 */

#include "EntityView.hpp"

#include <logic/Bomb.hpp>
#include <logic/Character.hpp>
#include <logic/PowerUp.hpp>
#include <logic/Wall.hpp>

#include <algorithm>
#include <filesystem>

namespace View {

namespace {

constexpr float kAtlasTileSize = 32.f;
constexpr int kWallTileSize = 16;

sf::IntRect tileRect(int column, int row) {
    return {column * static_cast<int>(kAtlasTileSize), row * static_cast<int>(kAtlasTileSize),
            static_cast<int>(kAtlasTileSize), static_cast<int>(kAtlasTileSize)};
}

sf::IntRect wallTileRect(int column, int row) {
    return {column * kWallTileSize, row * kWallTileSize, kWallTileSize, kWallTileSize};
}

const sf::Texture& spriteAtlas() {
    static sf::Texture texture;
    static bool loaded = false;

    if (!loaded) {
        loaded = true;

        const char* candidates[] = {
            "Sprites.png",
            "../Sprites.png",
            "../../Sprites.png",
        };

        for (const char* candidate : candidates) {
            if (std::filesystem::exists(candidate) && texture.loadFromFile(candidate)) {
                break;
            }
        }
    }

    return texture;
}

const sf::Texture& wallTilesAtlas() {
    static sf::Texture texture;
    static bool loaded = false;

    if (!loaded) {
        loaded = true;

        const char* candidates[] = {
            "Walltiles.png",
            "../Walltiles.png",
            "../../Walltiles.png",
        };

        for (const char* candidate : candidates) {
            if (std::filesystem::exists(candidate) && texture.loadFromFile(candidate)) {
                break;
            }
        }
    }

    return texture;
}

bool hasSpriteAtlas() {
    return spriteAtlas().getSize().x != 0U;
}

bool hasWallTilesAtlas() {
    return wallTilesAtlas().getSize().x != 0U;
}

void configureSprite(sf::Sprite& sprite, const sf::Texture& texture, const sf::IntRect& rect,
                     const sf::Vector2f& size, const sf::Vector2f& center,
                     const sf::Color& tint = sf::Color::White, float rotation = 0.f) {
    sprite.setTexture(texture, true);
    sprite.setTextureRect(rect);
    sprite.setOrigin(static_cast<float>(rect.width) * 0.5f, static_cast<float>(rect.height) * 0.5f);
    sprite.setScale(size.x / static_cast<float>(rect.width),
                    size.y / static_cast<float>(rect.height));
    sprite.setPosition(center);
    sprite.setRotation(rotation);
    sprite.setColor(tint);
}

void configureSprite(sf::Sprite& sprite, const sf::IntRect& rect, const sf::Vector2f& size,
                     const sf::Vector2f& center, const sf::Color& tint = sf::Color::White,
                     float rotation = 0.f) {
    configureSprite(sprite, spriteAtlas(), rect, size, center, tint, rotation);
}

const sf::IntRect kIndestructibleWallRect = wallTileRect(0, 0);
const sf::IntRect kDestructibleWallRect{16, 209, 16, 15};

const sf::IntRect kCharacterRect = tileRect(0, 1);
const sf::IntRect kCharacterAltRect = tileRect(0, 12);
const sf::IntRect kBombRect = tileRect(7, 22);
const sf::IntRect kFirePowerUpRect = tileRect(0, 22);
const sf::IntRect kExtraBombPowerUpRect = tileRect(1, 22);
const sf::IntRect kSkatesPowerUpRect = tileRect(2, 22);

// Sizes
constexpr float kCharacterSpriteScale = 2.0f;
constexpr float kBombSpriteScale = 2.0f;
constexpr float kPowerUpSpriteScale = 2.0f;

}

EntityView::EntityView(const std::shared_ptr<Logic::Entity>& model, Logic::Camera& camera)
    : m_model(model), m_camera(camera) {}

void EntityView::onGameEvent(const Logic::GameEvent& event) {
    if (event.type == Logic::GameEventType::EntityRemoved) {
        m_visible = false;
        return;
    }

    if (event.type == Logic::GameEventType::EntityMoved ||
        event.type == Logic::GameEventType::WorldTick) {
        syncFromModel();
    }
}

sf::Vector2f EntityView::toPixels(const Logic::Vector2& worldPosition) const {
    const Logic::Vector2 pixel = m_camera.project(worldPosition);
    return {pixel.x, pixel.y};
}

sf::Vector2f EntityView::toPixelSize(const Logic::Vector2& worldSize) const {
    const Logic::Vector2 pixelSize = m_camera.projectSize(worldSize);
    return {pixelSize.x, pixelSize.y};
}

std::shared_ptr<Logic::Entity> EntityView::lockModel() const {
    return m_model.lock();
}

WallView::WallView(const std::shared_ptr<Logic::Entity>& model, Logic::Camera& camera)
    : EntityView(model, camera), m_sprite() {
    syncFromModel();
}

void WallView::syncFromModel() {
    if (!hasWallTilesAtlas()) {
        m_visible = false;
        return;
    }

    const auto model = lockModel();
    if (!model || !model->isActive()) {
        m_visible = false;
        return;
    }

    const auto wall = std::static_pointer_cast<Logic::Wall>(model);
    const sf::Vector2f size = toPixelSize(model->getHalfExtents() * 2.f);
    const sf::Vector2f center = toPixels(model->getPosition());
    const sf::IntRect& rect =
        wall->isIndestructible() ? kIndestructibleWallRect : kDestructibleWallRect;

    configureSprite(m_sprite, wallTilesAtlas(), rect, size, center);
    m_visible = true;
}

void WallView::render(sf::RenderTarget& target) const {
    if (m_visible) {
        target.draw(m_sprite);
    }
}

CharacterView::CharacterView(const std::shared_ptr<Logic::Entity>& model, Logic::Camera& camera)
    : EntityView(model, camera), m_sprite() {
    syncFromModel();
}

void CharacterView::syncFromModel() {
    if (!hasSpriteAtlas()) {
        m_visible = false;
        return;
    }

    const auto model = lockModel();
    if (!model || !model->isActive()) {
        m_visible = false;
        return;
    }

    const auto character = std::static_pointer_cast<Logic::Character>(model);
    if (!character->isAlive()) {
        m_visible = false;
        return;
    }

    const sf::Vector2f size = toPixelSize(model->getHalfExtents() * 2.f);
    const sf::Vector2f center = toPixels(model->getPosition());

    configureSprite(m_sprite, character->isPlayer() ? kCharacterRect : kCharacterAltRect,
                    size * kCharacterSpriteScale, center,
                    character->isPlayer() ? sf::Color(255, 255, 255) : sf::Color(255, 210, 210));
    m_visible = true;
}

void CharacterView::render(sf::RenderTarget& target) const {
    if (m_visible) {
        target.draw(m_sprite);
    }
}

BombView::BombView(const std::shared_ptr<Logic::Entity>& model, Logic::Camera& camera)
    : EntityView(model, camera), m_sprite() {
    syncFromModel();
}

void BombView::syncFromModel() {
    if (!hasSpriteAtlas()) {
        m_visible = false;
        return;
    }

    const auto model = lockModel();
    if (!model || !model->isActive()) {
        m_visible = false;
        return;
    }

    const auto bomb = std::static_pointer_cast<Logic::Bomb>(model);
    if (bomb->isExploded()) {
        m_visible = false;
        return;
    }

    const sf::Vector2f size = toPixelSize(model->getHalfExtents() * 2.f);
    const sf::Vector2f center = toPixels(model->getPosition());
    const float fuseRatio = bomb->getRemainingFuse() / bomb->getFuseDuration();

    configureSprite(m_sprite, kBombRect, size * kBombSpriteScale, center,
                    sf::Color(255, 255, 255, static_cast<sf::Uint8>(180 + fuseRatio * 75)));
    m_visible = true;
}

void BombView::render(sf::RenderTarget& target) const {
    if (m_visible) {
        target.draw(m_sprite);
    }
}

PowerUpView::PowerUpView(const std::shared_ptr<Logic::Entity>& model, Logic::Camera& camera)
    : EntityView(model, camera), m_sprite() {
    syncFromModel();
}

void PowerUpView::syncFromModel() {
    if (!hasSpriteAtlas()) {
        m_visible = false;
        return;
    }

    const auto model = lockModel();
    if (!model || !model->isActive()) {
        m_visible = false;
        return;
    }

    const auto powerUp = std::static_pointer_cast<Logic::PowerUp>(model);
    const sf::Vector2f size = toPixelSize(model->getHalfExtents() * 2.f);
    const sf::Vector2f center = toPixels(model->getPosition());

    const sf::IntRect rect = [&] {
        switch (powerUp->getPowerUpType()) {
        case Logic::PowerUpType::Fire:
            return kFirePowerUpRect;
        case Logic::PowerUpType::ExtraBomb:
            return kExtraBombPowerUpRect;
        case Logic::PowerUpType::Skates:
            return kSkatesPowerUpRect;
        }
        return kFirePowerUpRect;
    }();

    configureSprite(m_sprite, rect, size * kPowerUpSpriteScale, center);
    m_visible = true;
}

void PowerUpView::render(sf::RenderTarget& target) const {
    if (m_visible) {
        target.draw(m_sprite);
    }
}

}
