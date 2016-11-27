#ifndef DREBBEL_H
#define DREBBEL_H

void handle_input(sf::Event& event, sf::RenderWindow& window, sf::Sprite& sprite);
bool detect_collision(const sf::Sprite& sprite_1, const sf::Sprite& sprite_2, sf::Uint8 alpha_limit);

#endif
