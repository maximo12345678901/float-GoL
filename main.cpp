#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

uint windowWidth = 1200;
uint windowHeight = 1000;

int gridWidth = 120;
int gridHeight = 100;

std::vector<std::vector<float>> cellStates(gridWidth, std::vector<float>(gridHeight, 0));
std::vector<std::vector<float>> newStates(gridWidth, std::vector<float>(gridHeight, 0));


int main() {
  sf::Image image(sf::Vector2u(gridWidth, gridHeight));
  sf::Texture texture;
  texture.loadFromImage(image);
  sf::Sprite sprite(texture);
  sprite.setScale(sf::Vector2f((float)windowWidth / (float)gridWidth, (float)windowHeight / (float)gridHeight));

  sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "lively game of life");
  window.setFramerateLimit(10);

  while(window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }

    for (int x = 0; x < gridWidth; x++) {
      for (int y = 0; y < gridHeight; y++) {
        float value = 0.0;

        newStates.at(x).at(y) = value;
        image.setPixel(sf::Vector2u(x, y), sf::Color(value * 255.0f, value * 255.0f, value * 255.0f));
      }
    }

    texture.update(image);

    std::swap(cellStates, newStates);
    
    window.clear();
    window.draw(sprite);
    window.display();
  }

  return 0;
}
