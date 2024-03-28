#ifndef MANDELBROT_HEADER
#define MANDELBROT_HEADER
#include <immintrin.h>

#include <SFML/Graphics.hpp>

static const size_t kWindowHeight = 720;
static const size_t kWindowWidth  = 1280;

#ifdef SFML_BUFFER
void PrintMandelbrot(sf::RenderWindow &window);

#else
void PrintMandelbrot(sf::RenderWindow &window,
                     sf::Uint8        *pixel_array);
#endif

/*
void DIY_AVX_PrintMandelbrot(sf::RenderWindow &window);
*/

#ifdef SFML_BUFFER
void AVX_PrintMandelbrot(sf::RenderWindow &window);

#else
void AVX_PrintMandelbrot(sf::RenderWindow &window,
                         sf::Uint8        *pixel_array);
#endif

#endif
