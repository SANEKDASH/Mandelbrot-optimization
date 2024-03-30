#ifndef MANDELBROT_HEADER
#define MANDELBROT_HEADER
#include <immintrin.h>

#include <SFML/Graphics.hpp>

#ifdef SPEED_TEST

    #define SPEED_TEST_ONLY(...) __VA_ARGS__

#else

    #define SPEED_TEST_ONLY(...) ;

#endif

static const size_t kWindowHeight = 720;
static const size_t kWindowWidth  = 1280;

static const size_t kColorAttrNumber = 4;

struct ViewProperties
{
    float scale;
    float x_shift;
    float y_shift;
};

typedef enum MandelbrotErrs
{
    kMandelbrotSuccess,
    kNullPixelArray,
};

MandelbrotErrs PrintMandelbrot(sf::RenderWindow &window,
                               sf::Uint8        *pixel_array,
                               ViewProperties   *view_properties);

MandelbrotErrs AVX_PrintMandelbrot(sf::RenderWindow &window,
                                   sf::Uint8        *pixel_array,
                                   ViewProperties   *view_properties);

#endif
