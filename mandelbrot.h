#include <SFML/Graphics.hpp>

static const size_t kWindowHeight = 720;
static const size_t kWindowWidth  = 1280;

void PrintMandelbrot(sf::RenderWindow &window);

void DIY_AVX_PrintMandelbrot(sf::RenderWindow &window);

void AVX_PrintMandelbrot(sf::RenderWindow &window);
