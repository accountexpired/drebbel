#include <iostream>
#include <memory>

#include <SFML/Graphics.hpp>

#include "drebbel.hpp"

namespace
{
    namespace Local
    {
        const int window_x_res = 800;
        const int window_y_res = 600;
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

class Entity
{
protected:
    int x_coord;
    int y_coord;
    int parallax_depth;
    sf::Texture texture;
    sf::Sprite sprite;

public:
    Entity(const std::string& image, const int x_pos, const int y_pos);
    virtual ~Entity() = 0;
    virtual void move(int x_coord, int y_coord);
    sf::Sprite& get_sprite();
    int get_parallax_depth() const;
    void set_parallax_depth(int depth);
};

Entity::Entity(const std::string& image, const int x_pos, const int y_pos)
    : parallax_depth(1)
{
    load_sprite(texture, image, sprite);
    sprite.setPosition(x_pos, y_pos);
    x_coord = x_pos;
    y_coord = y_pos;
}

Entity::~Entity()
{
}

void Entity::move(int x_coord, int y_coord)
{
    sprite.move(sf::Vector2f(x_coord, y_coord));
}

int Entity::get_parallax_depth() const
{
    return parallax_depth;
}

void Entity::set_parallax_depth(int depth)
{
    parallax_depth = depth;
}

sf::Sprite& Entity::get_sprite()
{
    return sprite;
}

class Submarine : public Entity
{
    using Entity::Entity;

public:
    void move(int x_coord, int y_coord);
};

void Submarine::move(int x_coord, int y_coord)
{
}

class Mountain : public Entity
{
    using Entity::Entity;
};

class World
{
    std::vector<std::unique_ptr<Entity>> entities;
public:
    void add_entity(std::unique_ptr<Entity>);
    void draw_entities(sf::RenderWindow& window) const;
    void move(const int x_coord, const int y_coord);
};

void World::add_entity(std::unique_ptr<Entity> entity)
{
    entities.push_back(std::move(entity));
}

void World::draw_entities(sf::RenderWindow& window) const
{
    for (const auto& entity : entities)
    {
        window.draw(entity->get_sprite());
    }
}

void World::move(const int x_coord, const int y_coord)
{
    for (const auto& entity : entities)
    {
        entity->move(x_coord / entity->get_parallax_depth(), y_coord);
    }
}

void handle_input(sf::Event& event, sf::RenderWindow& window, World& world)
{
    if (event.type == sf::Event::Closed)
    {
        window.close();
    }

    else if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Left)
        {
            world.move(10, 0);
        }
        else if (event.key.code == sf::Keyboard::Right)
        {
            world.move(-10, 0);
        }
        else if (event.key.code == sf::Keyboard::Up)
        {
            world.move(0, 10);
        }
        else if (event.key.code == sf::Keyboard::Down)
        {
            world.move(0, -10);
        }
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(Local::window_x_res, Local::window_y_res), "Drebbel");

    World world;

    // Position the submarine a slightly left of the center of the window.
    auto sub = std::make_unique<Submarine>(
        "images/submarine.png",
        Local::window_x_res / 2 - Local::window_x_res * 0.1,
        Local::window_y_res / 2);

    auto iceberg_fg = std::make_unique<Mountain>("images/iceberg_fg.png", 100, 300);

    auto iceberg_bg = std::make_unique<Mountain>("images/iceberg_bg.png", 500, 200);
    iceberg_bg->set_parallax_depth(3);

    world.add_entity(std::move(iceberg_bg));
    world.add_entity(std::move(iceberg_fg));
    world.add_entity(std::move(sub));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            handle_input(event, window, world);

            window.clear(sf::Color::Blue);
            world.draw_entities(window);
            window.display();
        }
    }
}
