#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <string>

namespace View {

/**
 * @brief Draws a single line of text centered horizontally at the given y coordinate.
 * @param target Render surface.
 * @param font Typeface to use.
 * @param text String content.
 * @param characterSize Font size in pixels.
 * @param color Text color.
 * @param centerX Horizontal center position in pixels.
 * @param y Baseline y position in pixels.
 */
void drawCenteredText(sf::RenderTarget& target, const sf::Font& font, const std::string& text,
                      unsigned int characterSize, sf::Color color, float centerX, float y);

/**
 * @brief Draws a filled rectangle with an outline (used for buttons and panels).
 * @param target Render surface.
 * @param rect Axis-aligned rectangle in pixel space.
 * @param fill Interior color.
 * @param outline Border color.
 */
void drawPanel(sf::RenderTarget& target, const sf::FloatRect& rect, sf::Color fill,
               sf::Color outline);

}
