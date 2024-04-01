#include <stdlib.h>
#include <math.h>

#include "mandelbrot.h"

//==================================================================================

static const size_t kMaxIterationCount = 256;

static const float kMaxRadius = 100.f;

const float kDevX = 0.005;
const float kDevY = 0.005;

const char kMyLovelyColorRedAttr   = 132;
const char kMyLovelyColorGreenAttr = 72;
const char kMyLovelyColorBlueAttr  = 178;

const char kRedColorMultiplier   = 6;
const char kGreenColorMultiplier = 4;
const char kBlueColorMultiplier  = 9;

const size_t kVectorSize = 8;

const char kMaxOpacityLevel = 255;

//==================================================================================

static void SetPointsInPixelBuffer(sf::Uint8    *pixel_array,
                                   int           int_x,
                                   int           int_y,
                                   int          *iteration_count_ptr,
                                   const size_t  vector_size);

static void PrintPixelsInSFMLBuffer(sf::RenderWindow &window,
                                    int               int_x,
                                    int               int_y,
                                    int              *iteration_count_ptr,
                                    const size_t      vector_size);

//==================================================================================

MandelbrotErrs PrintMandelbrot(sf::RenderWindow &window,
                               sf::Uint8        *pixel_array,
                               ViewProperties   *view_properties)
{
    sf::RectangleShape pixel(sf::Vector2f(1.f, 1.f));

    for (size_t int_y = 0; int_y < kWindowHeight; int_y++)
    {
        float curr_y = ((float) kWindowHeight / 2 - (float) int_y ) * kDevY * view_properties->scale + view_properties->y_shift * kDevY;

        for (size_t int_x = 0; int_x < kWindowWidth; int_x++)
        {
            float curr_x = ((float) int_x - (float) kWindowWidth / 2) * kDevX * view_properties->scale +
                            view_properties->x_shift * kDevX;

            int iteration_count = 0;

            float new_x = curr_x;
            float new_y = curr_y;

            for (; iteration_count < kMaxIterationCount; iteration_count++)
            {
                float square_x = new_x * new_x;
                float square_y = new_y * new_y;
                float x_y      = new_x * new_y;
                float radius_2 = square_x + square_y;

                if (radius_2 >= kMaxRadius)
                {
                    break;
                }

                new_x = square_x - square_y + curr_x;

                new_y = x_y + x_y + curr_y;
            }
            #ifdef SFML_BUFFER

                PrintPixelsInSFMLBuffer(window,
                                        int_x,
                                        int_y,
                                        &iteration_count,
                                        1);
            #else

                if (pixel_array != nullptr)
                {
                    SetPointsInPixelBuffer(pixel_array,
                                           int_x,
                                           int_y,
                                           &iteration_count,
                                           1);
                }
                else
                {
                    return kNullPixelArray;
                }

            #endif
        }
    }

    return kMandelbrotSuccess;
}

//==================================================================================

MandelbrotErrs AVX_PrintMandelbrot(sf::RenderWindow &window,
                                   sf::Uint8        *pixel_array,
                                   ViewProperties   *view_properties)
{
    const float d_x = 0.005;
    const float d_y = 0.005;

    const __m256 max_square_radius = _mm256_set1_ps(100.f);

    const __m256 _255  = _mm256_set1_ps(255.f);

    const __m256 _76543210 = _mm256_set_ps(7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);

    for (size_t int_y = 0; int_y < kWindowHeight; int_y++)
    {
        float curr_y =  ((float) kWindowHeight / 2 - (float) int_y) * kDevY * view_properties->scale + view_properties->y_shift * kDevY;

        float curr_x = - (float) kWindowWidth  / 2;

        for (size_t int_x = 0; int_x < kWindowWidth; int_x += kVectorSize)
        {
            __m256 start_x = _mm256_add_ps(_mm256_mul_ps(_mm256_add_ps(_76543210,
                                                                       _mm256_add_ps(_mm256_set1_ps((float) int_x),
                                                                                     _mm256_set1_ps(        curr_x))),
                                                         _mm256_set1_ps(kDevX * view_properties->scale)),
                                           _mm256_set1_ps(view_properties->x_shift * kDevX));

/*            __m256 start_x = _mm256_mul_ps(_mm256_add_ps(_76543210,
                                                         _mm256_add_ps(_mm256_set1_ps((float) int_x),
                                                                       _mm256_set1_ps(        curr_x))),
                                           _mm256_set1_ps(kDevX * view_properties->scale));
*/
            __m256 start_y = _mm256_set1_ps(curr_y);

            __m256 new_x = start_x;
            __m256 new_y = start_y;

            __m256i iteration_count_array = _mm256_setzero_si256();

            for (size_t i = 0; i < kMaxIterationCount; i++)
            {
                __m256 square_x = _mm256_mul_ps(new_x, new_x);
                __m256 square_y = _mm256_mul_ps(new_y, new_y);

                __m256 radius_2 = _mm256_add_ps(square_x, square_y);

                __m256 cmp = _mm256_cmp_ps(radius_2, max_square_radius, _CMP_LE_OS);

                int mask = _mm256_movemask_ps(cmp);

                if (mask == 0)
                {
                    break;
                }

                iteration_count_array = _mm256_sub_epi32(iteration_count_array, _mm256_castps_si256(cmp));

                __m256 x_y = _mm256_mul_ps(new_x, new_y);

                new_x = _mm256_add_ps(_mm256_sub_ps(square_x, square_y), start_x);
                new_y = _mm256_add_ps(_mm256_add_ps(x_y     , x_y     ), start_y);
            }

            int *iteration_count_ptr = (int *) &iteration_count_array;

            #ifdef SFML_BUFFER

                PrintPixelsInSFMLBuffer(window,
                                        int_x,
                                        int_y,
                                        iteration_count_ptr,
                                        kVectorSize);
            #else

                if (pixel_array != nullptr)
                {
                    SetPointsInPixelBuffer(pixel_array,
                                           int_x,
                                           int_y,
                                           iteration_count_ptr,
                                           kVectorSize);
                }
                else
                {
                    return kNullPixelArray;
                }

            #endif
        }

    }

    return kMandelbrotSuccess;
}

//==================================================================================

static inline void SetPointsInPixelBuffer(sf::Uint8    *pixel_array,
                                          int           int_x,
                                          int           int_y,
                                          int          *iteration_count_ptr,
                                          const size_t  vector_size)
{
    for (size_t i = 0; i < vector_size; i++)
    {
        size_t pixel_array_pos = (int_y * kWindowWidth + int_x + i) * 4;

        sf::Uint8 red_color   = 0;
        sf::Uint8 green_color = 0;
        sf::Uint8 blue_color  = 0;

        if (iteration_count_ptr[i] < kMaxIterationCount)
        {
            red_color   = kMyLovelyColorRedAttr   - (char) iteration_count_ptr[i] * kRedColorMultiplier;
            green_color = kMyLovelyColorGreenAttr - (char) iteration_count_ptr[i] * kGreenColorMultiplier;
            blue_color  = kMyLovelyColorBlueAttr  - (char) iteration_count_ptr[i] * kBlueColorMultiplier;
        }

        pixel_array[pixel_array_pos    ] = red_color;
        pixel_array[pixel_array_pos + 1] = green_color;
        pixel_array[pixel_array_pos + 2] = blue_color;
        pixel_array[pixel_array_pos + 3] = kMaxOpacityLevel;
    }
}

//==================================================================================

static inline void PrintPixelsInSFMLBuffer(sf::RenderWindow &window,
                                           int               int_x,
                                           int               int_y,
                                           int              *iteration_count_ptr,
                                           const size_t      vector_size)
{
    sf::RectangleShape pixel(sf::Vector2f(1.f, 1.f));

    for (size_t i = 0; i < vector_size; i++)
    {
        pixel.setPosition(int_x + i, int_y);

        if (iteration_count_ptr[i] >= kMaxIterationCount)
        {
            pixel.setFillColor(sf::Color::Black);
        }
        else
        {
            pixel.setFillColor(sf::Color(kMyLovelyColorRedAttr   - (char) iteration_count_ptr[i] * kRedColorMultiplier,
                                         kMyLovelyColorGreenAttr - (char) iteration_count_ptr[i] * kGreenColorMultiplier,
                                         kMyLovelyColorBlueAttr  - (char) iteration_count_ptr[i] * kBlueColorMultiplier));
        }

        window.draw(pixel);
    }
}
