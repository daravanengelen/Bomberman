#include <logic/Random.hpp>
#include <logic/Stopwatch.hpp>

#include <SFML/Graphics.hpp>

int main() {
    auto& stopwatch = Logic::Stopwatch::getInstance();
    auto& random = Logic::Random::getInstance();

    stopwatch.reset();

    sf::RenderWindow window(
        sf::VideoMode(800, 600),
        "Bomberman",
        sf::Style::Close
    );
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        stopwatch.tick();
        const float dt = stopwatch.getDeltaTime();

        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        (void)random.range(0, 3);
        (void)dt;

        window.clear(sf::Color(30, 30, 40));
        window.display();
    }

    return 0;
}
