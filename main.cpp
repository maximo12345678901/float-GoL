#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cmath>
#include <random>
#include <omp.h>
#include <iostream>

uint windowWidth = 1600;
uint windowHeight = 900;

int gridWidth = 800;
int gridHeight = 450;

int t = 0;

std::vector<std::vector<float>> cellStates(gridWidth, std::vector<float>(gridHeight, 0));
std::vector<std::vector<float>> newStates(gridWidth, std::vector<float>(gridHeight, 0));
std::vector<std::vector<float>> convolutionGrid {
  // {0.1f, -0.1f, 0.1f},
  // {-0.1f, 1.0f, -0.1f},
  // {0.1f, -0.1f, 0.1f}

  // {1.0f, 1.0f, 1.0f},
  // {1.0f, 9.0f, 1.0f},
  // {1.0f, 1.0f, 1.0f}

  {0.68f, -0.9f, 0.68f},
  {-0.9f, -0.66f, -0.9f},
  {0.68f, -0.9f, 0.68f}
  
};

float activation (float x) {
  // if (x == 3.0f || x == 4.0f || x == 11.0f || x == 12.0f) {
  //   x = 1.0f;
  // }
  // else {
  //   x = 0.0f;
  // }
  
  // if (x < 0) {x = 0;}
  // if (x > 1) {x = 1;}

  return -1.0f/std::pow(2.0f, (0.6f*std::pow(x, 2.0f)))+1.0f;
  // return x;
}

float nextCellValue(int x, int y) {
  float convolvedValue = 0.0f;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (x + i < 0 || x + i >= gridWidth) {continue;}
      if (y + j < 0 || y + j >= gridHeight) {continue;}
      
      convolvedValue += convolutionGrid.at(i + 1).at(j + 1) * cellStates.at(x + i).at(y + j);
    }
  }

  return activation(convolvedValue);
}

void mouseDraw(int radius, sf::RenderWindow& window) {
  sf::Vector2i mousePos = sf::Mouse::getPosition(window);
  int x = mousePos.x / ((float)windowWidth / (float)gridWidth);
  int y = mousePos.y / (windowHeight / gridHeight);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
    for (int i = -radius; i <= radius; i++) {
      for (int j = -radius; j <= radius; j++) {
        if (x + i < 0 || x + i >= gridWidth) {continue;}
        if (y + j < 0 || y + j >= gridHeight) {continue;}

        cellStates.at(x + i).at(y + j) = 1.0f;
      }
    }
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
    for (int i = -radius; i <= radius; i++) {
      for (int j = -radius; j <= radius; j++) {
        if (x + i < 0 || x + i >= gridWidth) {continue;}
        if (y + j < 0 || y + j >= gridHeight) {continue;}

        cellStates.at(x + i).at(y + j) = 0.0f;
      }
    }
  }
}

std::mt19937 rng(std::random_device{}());
std::uniform_real_distribution<float> dist(0.0f, 1.0f);

void fillRandomly(float min, float max) {
  for (int x = 0; x < gridWidth; x++) {
    for (int y = 0; y < gridHeight; y++) {
      cellStates.at(x).at(y) = min + dist(rng) * (max - min);
    }
  }
}

int main() {
  sf::Image image(sf::Vector2u(gridWidth, gridHeight));
  sf::Texture texture;
  texture.loadFromImage(image);
  sf::Sprite sprite(texture);
  sprite.setScale(sf::Vector2f((float)windowWidth / (float)gridWidth, (float)windowHeight / (float)gridHeight));

  sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "lively game of life");
  window.setFramerateLimit(240);

  omp_set_num_threads(6);

  //                -- loop -- 
  while(window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }
    t++;

    mouseDraw(2, window);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
      fillRandomly(0.0f, 1.0f);
    }

    //   update grid
    if (t % 4 == 0) {
      #pragma omp parallel for collapse(2)
      for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
          float value = nextCellValue(x, y);
          newStates.at(x).at(y) = value;
          image.setPixel(sf::Vector2u(x, y), sf::Color(value * 255.0f, value * 255.0f, value * 255.0f));
        }
      }
    if (t % 2 == 0) {
      texture.update(image);
    }

    std::swap(cellStates, newStates);
    }
    
    window.clear();
    window.draw(sprite);
    window.display();
  }

  return 0;
}
