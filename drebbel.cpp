#include <iostream>

#include <SFML/Graphics.hpp>

#include "drebbel.hpp"

void handle_input(sf::Event& event, sf::RenderWindow& window, sf::Sprite& sprite)
{
    if (event.type == sf::Event::Closed)
    {
        window.close();
    }
    else if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Left)
        {
            sprite.move(sf::Vector2f(-10, 0));
        }
        else if (event.key.code == sf::Keyboard::Right)
        {
            sprite.move(sf::Vector2f(10, 0));
        }
        else if (event.key.code == sf::Keyboard::Up)
        {
            sprite.move(sf::Vector2f(0, -10));
        }
        else if (event.key.code == sf::Keyboard::Down)
        {
            sprite.move(sf::Vector2f(0, 10));
        }
    }
}

bool load_sprite(sf::Texture& texture, const std::string& texture_path, sf::Sprite& sprite)
{
    if (!texture.loadFromFile(texture_path))
    {
        std::cerr << "Unable to load texture: %s" << texture_path << std::endl;
        return false;
    }
    texture.setSmooth(true);
    sprite.setTexture(texture);

    return true;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Drebbel");

    sf::Texture sub_texture;
    sf::Sprite sub;
    load_sprite(sub_texture, "images/submarine.png", sub);

    sf::Texture iceberg_texture;
    sf::Sprite iceberg;
    load_sprite(iceberg_texture, "images/iceberg.png", iceberg);

    iceberg.setPosition(100, 300);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            handle_input(event, window, sub);

            window.clear(sf::Color::Blue);
            window.draw(sub);
            window.draw(iceberg);
            window.display();
        }
    }
}
