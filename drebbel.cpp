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

class Entity
{
protected:
    int x_coord;
    int y_coord;
    int parallax_depth;
    sf::Sprite sprite;

public:
    Entity(const sf::Texture& texture, const int x_pos, const int y_pos);
    virtual ~Entity() = 0;
    virtual void move(int x_coord, int y_coord);
    sf::Sprite& get_sprite();
    int get_parallax_depth() const;
    void set_parallax_depth(int depth);
};

Entity::Entity(const sf::Texture& texture, const int x_pos, const int y_pos)
    : parallax_depth(1)
{
    sprite.setTexture(texture);
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

class Submarine final : public Entity
{
    using Entity::Entity;

public:
    void move(int x_coord, int y_coord);
};

void Submarine::move(int x_coord, int y_coord)
{
    // Silence static code analyser.
    (void)x_coord;
    (void)y_coord;
}

class Mountain final : public Entity
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

        // Check if the new position results in a collision.
        for (const auto& collision_candidate : entities)
        {
            if (entity != collision_candidate && entity->get_parallax_depth() == collision_candidate->get_parallax_depth())
            {
                if (detect_collision(entity->get_sprite(), collision_candidate->get_sprite(), 16))
                {
                    std::cout << "Collision!" << std::endl;
                }
            }
        }
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

class BitmaskManager
{
public:
    ~BitmaskManager()
    {
        std::map<const sf::Texture*, sf::Uint8*>::const_iterator end = bitmasks.end();
        for (std::map<const sf::Texture*, sf::Uint8*>::const_iterator iter = bitmasks.begin(); iter != end; ++iter)
        {
            delete [] iter->second;
        }
    }

    sf::Uint8 get_pixel(const sf::Uint8* mask, const sf::Texture* texture, unsigned int x, unsigned int y)
    {
        if (x > texture->getSize().x || y > texture->getSize().y)
        {
            return 0;
        }
        return mask[x + y  * texture->getSize().x];
    }

    sf::Uint8* create_mask(const sf::Texture* texture, const sf::Image& image)
    {
        sf::Uint8* mask = new sf::Uint8[texture->getSize().y * texture->getSize().x];

        for (unsigned int y = 0; y < texture->getSize().y; y++)
        {
            for (unsigned int x = 0; x < texture->getSize().x; x++)
            {
                mask[x + y * texture->getSize().x] = image.getPixel(x, y).a;
            }
        }
        bitmasks.insert(std::pair<const sf::Texture*, sf::Uint8*>(texture, mask));

        return mask;
    }

    sf::Uint8* get_mask(const sf::Texture* texture)
    {
        sf::Uint8* mask;
        std::map<const sf::Texture*, sf::Uint8*>::iterator pair = bitmasks.find(texture);
        if (pair == bitmasks.end())
        {
            sf::Image image = texture->copyToImage();
            mask = create_mask(texture, image);
        }
        else
        {
            mask = pair->second;
        }
        return mask;
    }
private:
    std::map<const sf::Texture*, sf::Uint8*> bitmasks;
};

bool detect_collision(const sf::Sprite& sprite_1, const sf::Sprite& sprite_2, sf::Uint8 alpha_limit)
{
    sf::FloatRect intersection;
    BitmaskManager bitmasks;

    // Do the two objects' rectangles intersect?
    if (sprite_1.getGlobalBounds().intersects(sprite_2.getGlobalBounds(), intersection))
    {
        sf::IntRect sprite_1_sub_rect = sprite_1.getTextureRect();
        sf::IntRect sprite_2_sub_rect = sprite_2.getTextureRect();

        sf::Uint8* mask_1 = bitmasks.get_mask(sprite_1.getTexture());
        sf::Uint8* mask_2 = bitmasks.get_mask(sprite_1.getTexture());

        // Do the pixels within the colliding rectangles intersect?
        for (int i = intersection.left; i < intersection.left + intersection.width; i++)
        {
            for (int j = intersection.top; j < intersection.top + intersection.height; j++)
            {
                sf::Vector2f sprite_1_vec = sprite_1.getInverseTransform().transformPoint(i, j);
                sf::Vector2f sprite_2_vec = sprite_2.getInverseTransform().transformPoint(i, j);

                if (sprite_1_vec.x > 0 && sprite_1_vec.y > 0 && sprite_2_vec.x > 0 && sprite_2_vec.y > 0 &&
                    sprite_1_vec.x < sprite_1_sub_rect.width && sprite_1_vec.y < sprite_1_sub_rect.height &&
                    sprite_2_vec.x < sprite_2_sub_rect.width && sprite_2_vec.y < sprite_2_sub_rect.height)
                {
                    // Are pixels within sprite's subrect?
                    if (bitmasks.get_pixel(mask_1, sprite_1.getTexture(),
                                           static_cast<int>(sprite_1_vec.x) + sprite_1_sub_rect.left,
                                           static_cast<int>(sprite_1_vec.y) + sprite_1_sub_rect.top) < alpha_limit &&
                        bitmasks.get_pixel(mask_2, sprite_2.getTexture(),
                                           static_cast<int>(sprite_2_vec.x) + sprite_2_sub_rect.left,
                                           static_cast<int>(sprite_2_vec.y) + sprite_2_sub_rect.top) < alpha_limit)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(Local::window_x_res, Local::window_y_res), "Drebbel");

    World world;

    sf::Texture sub_tex;
    sub_tex.loadFromFile("images/submarine.png");

    sf::Texture iceberg_near_tex;
    iceberg_near_tex.loadFromFile("images/iceberg_fg.png");

    sf::Texture iceberg_far_tex;
    iceberg_far_tex.loadFromFile("images/iceberg_bg.png");

    // Position the submarine a slightly left of the center of the window.
    auto sub = std::make_unique<Submarine>(
        sub_tex,
        Local::window_x_res / 2 - Local::window_x_res * 0.1,
        Local::window_y_res / 2);

    auto iceberg_fg = std::make_unique<Mountain>(iceberg_near_tex, 100, 300);

    auto iceberg_bg = std::make_unique<Mountain>(iceberg_far_tex, 500, 200);
    iceberg_bg->set_parallax_depth(3);

    auto iceberg_temp = std::make_unique<Mountain>(iceberg_far_tex, 300, 200);
    iceberg_temp->set_parallax_depth(4);

    world.add_entity(std::move(iceberg_temp));
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
