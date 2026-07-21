#include "Ui.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace View {

void drawCenteredText(sf::RenderTarget& target, const sf::Font& font, const std::string& text,
                      unsigned int characterSize, sf::Color color, float centerX, float y) {
    sf::Text label;
    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(characterSize);
    label.setFillColor(color);
    const sf::FloatRect bounds = label.getLocalBounds();
    label.setOrigin(bounds.width * 0.5f, bounds.height * 0.5f);
    label.setPosition(centerX, y);
    target.draw(label);
}

void drawPanel(sf::RenderTarget& target, const sf::FloatRect& rect, sf::Color fill,
               sf::Color outline) {
    sf::RectangleShape panel({rect.width, rect.height});
    panel.setPosition(rect.left, rect.top);
    panel.setFillColor(fill);
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(outline);
    target.draw(panel);
}

}
