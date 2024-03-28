#include <stdlib.h>
#include <math.h>

#include "mandelbrot.h"

static const size_t kMaxIterationCount = 256;

static const float kMaxRadius = 100.f;

const float ROI_x = -1.325f;
const float ROI_y = 0;

const float dx = 0.005;
const float dy = 0.005;

const size_t kVectorSize = 16;

#ifdef SFML_BUFFER

void PrintMandelbrot(sf::RenderWindow &window)

#else

void PrintMandelbrot(sf::RenderWindow &window,
                     sf::Uint8        *pixel_array)

#endif
{
    sf::RectangleShape pixel(sf::Vector2f(1.f, 1.f));

    for (size_t int_y = 0; int_y < kWindowHeight; int_y++)
    {
        float curr_y = ((float) kWindowHeight / 2 - (float) int_y) * dy;

        for (size_t int_x = 0; int_x < kWindowWidth; int_x++)
        {
            float curr_x = ((float) int_x - (float) kWindowWidth / 2) * dx;

            size_t iteration_count = 0;

            float X = curr_x;
            float Y = curr_y;

            for (; iteration_count < kMaxIterationCount; iteration_count++)
            {
                float square_x = X * X;
                float square_y = Y * Y;
                float x_y      = X * Y;
                float radius_2 = square_x + square_y;

                if (radius_2 >= kMaxRadius)
                {
                    break;
                }

                X = square_x - square_y + curr_x;
                Y = x_y + x_y           + curr_y;
            }

            #ifdef SFML_BUFFER

            pixel.setPosition(int_x, int_y);

            if (iteration_count >= kMaxIterationCount)
            {
                pixel.setFillColor(sf::Color::Black);
            }
            else
            {
                pixel.setFillColor(sf::Color(132 - (char) iteration_count * 6,
                                             72 - (char) iteration_count * 4,
                                             178 - (char) iteration_count * 9));
            }
            #else

                size_t pixel_array_pos = (int_y * kWindowWidth + int_x) * 4;

                sf::Uint8 red_color   = 0;
                sf::Uint8 green_color = 0;
                sf::Uint8 blue_color  = 0;

                if (iteration_count < kMaxIterationCount)
                {
                    red_color   = 132 - (char) iteration_count * 6;
                    green_color = 72  - (char) iteration_count * 4;
                    blue_color  = 178 - (char) iteration_count * 9;
                }

                pixel_array[pixel_array_pos    ] = red_color;
                pixel_array[pixel_array_pos + 1] = green_color;
                pixel_array[pixel_array_pos + 2] = blue_color;
                pixel_array[pixel_array_pos + 3] = 255;//what???

            #endif

            window.draw(pixel);

        }
    }
}



static inline void mm_set_ps(float mm[kVectorSize],
                             float val0,
                             float val1,
                             float val2,
                             float val3,
                             float val4,
                             float val5,
                             float val6,
                             float val7,
                             float val8,
                             float val9,
                             float val10,
                             float val11,
                             float val12,
                             float val13,
                             float val14,
                             float val15)
{
    mm[0] = val0;
    mm[1] = val1;
    mm[2] = val2;
    mm[3] = val3;
    mm[4] = val4;
    mm[5] = val5;
    mm[6] = val6;
    mm[7] = val7;
    mm[8] = val8;
    mm[9] = val9;
    mm[10] = val10;
    mm[11] = val11;
    mm[12] = val12;
    mm[13] = val13;
    mm[14] = val14;
    mm[15] = val15;
}

static inline void mm_set_psl(float mm[kVectorSize],
                              float val)
{
    for (size_t i = 0; i < kVectorSize; i++)
    {
        mm[i] = val;
    }
}

static inline void mm_cpy_ps(float mm[kVectorSize],
                             float mm2[kVectorSize])
{
    for (size_t i = 0; i < kVectorSize; i++)
    {
        mm[i] = mm2[i];
    }
}

static inline void mm_add_ps(      float mm[kVectorSize],
                             const float mm1[kVectorSize],
                             const float mm2[kVectorSize])
{
    for (size_t i = 0; i < kVectorSize; i++)
    {
        mm[i] = mm1[i] + mm2[i];
    }
}

static inline void mm_sub_ps(      float mm[kVectorSize],
                             const float mm1[kVectorSize],
                             const float mm2[kVectorSize])
{
    for (size_t i = 0; i < kVectorSize; i++)
    {
        mm[i] = mm1[i] - mm2[i];
    }
}

static inline void mm_mul_ps(      float mm[kVectorSize],
                             const float mm1[kVectorSize],
                             const float mm2[kVectorSize])
{
    for (size_t i = 0; i < kVectorSize; i++)
    {
        mm[i] = mm1[i] * mm2[i];
    }
}

static inline void mm_add_epi32(      size_t mm[kVectorSize],
                                const size_t mm1[kVectorSize],
                                const float mm2[kVectorSize])
{
    for (size_t i = 0; i < kVectorSize; i++)
    {
        mm[i] = mm1[i] + !!mm2[i];
    }
}

static inline void mm_cmple_ps(      float cmp[kVectorSize],
                               const float mm1[kVectorSize],
                               const float mm2[kVectorSize])
{
    for (size_t i = 0; i < kVectorSize; i++)
    {
        if (mm1[i] <= mm2[i])
        {
            cmp[i] = -1;
        }
        else
        {
            cmp[i] = 0;
        }
    }
}

static inline int mm_movemask_ps(const float cmp[kVectorSize])
{
    int mask = 0;

    for (size_t i = 0; i < kVectorSize; i++)
    {
        mask |= (!!cmp[i] << i);
    }

    return mask;
}

void DIY_AVX_PrintMandelbrot(sf::RenderWindow &window)
{
    sf::RectangleShape pixel(sf::Vector2f(1.f, 1.f));

    float shift_array[kVectorSize] = {};

    for (size_t i = 0; i < kVectorSize; i++)
    {
        shift_array[i] = i;
    }

    float r2MAX[kVectorSize] = {}; mm_set_psl(r2MAX, kMaxRadius);

    for (size_t int_y = 0; int_y < kWindowHeight; int_y++)
    {
        float curr_y[kVectorSize] = {};

        mm_set_psl(curr_y, ((float) kWindowHeight / 2 - (float) int_y) * dy);

        for (size_t int_x = 0; int_x < kWindowWidth; int_x += kVectorSize)
        {
            float    curr_x[kVectorSize] = {};

            mm_set_ps(curr_x, ((float) int_x + 0  - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 1  - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 2  - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 3  - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 4  - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 5  - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 6  - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 7  - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 8  - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 9  - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 10 - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 11 - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 12 - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 13 - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 14 - (float) kWindowWidth / 2) * dx,
                              ((float) int_x + 15 - (float) kWindowWidth / 2) * dx);

            float X[kVectorSize] = {}; mm_cpy_ps(X, curr_x);
            float Y[kVectorSize] = {}; mm_cpy_ps(Y, curr_y);

            size_t iteration_count[kVectorSize] = {0};

            for (size_t n = 0; n < kMaxIterationCount; n++)
            {
                float square_x[kVectorSize] = {}; mm_mul_ps(square_x, X, X);
                float square_y[kVectorSize] = {}; mm_mul_ps(square_y, Y, Y);

                float radius_2[kVectorSize] = {}; mm_add_ps(radius_2, square_x, square_y);

                float cmp[kVectorSize] = {0}; mm_cmple_ps(cmp, radius_2, r2MAX);

                int mask = mm_movemask_ps(cmp);

                if (!mask)
                {
                    break;
                }

                mm_add_epi32(iteration_count,
                             iteration_count,
                             cmp);

                float      x_y[kVectorSize] = {}; mm_mul_ps(x_y     , X, Y);

                mm_add_ps(X, curr_x, square_x);
                mm_sub_ps(X, X     , square_y);

                mm_add_ps(Y, x_y, x_y);
                mm_add_ps(Y, Y, curr_y);
            }

            for (size_t i = 0; i < kVectorSize; i++)
            {
                pixel.setPosition(int_x + i, int_y);

                if (iteration_count[i] >= kMaxIterationCount)
                {
                    pixel.setFillColor(sf::Color::Black);
                }
                else
                {
                    pixel.setFillColor(sf::Color(132 - (char) iteration_count[i] * 6,
                                                 72  - (char) iteration_count[i] * 4,
                                                 178 - (char) iteration_count[i] * 9));
                }

                window.draw(pixel);
            }
        }
    }
}

#ifdef SFML_BUFFER

void AVX_PrintMandelbrot(sf::RenderWindow &window)

#else

void AVX_PrintMandelbrot(sf::RenderWindow &window,
                         sf::Uint8        *pixel_array)

#endif
{
    sf::RectangleShape pixel(sf::Vector2f(1.f, 1.f));

    const float d_x = 0.005;
    const float d_y = 0.005;

    const __m256 max_square_radius = _mm256_set1_ps(100.f);

    const __m256 _255  = _mm256_set1_ps(255.f);

    const __m256 _76543210 = _mm256_set_ps(7.f,
                                           6.f,
                                           5.f,
                                           4.f,
                                           3.f,
                                           2.f,
                                           1.f,
                                           0.f);

    for (size_t int_y = 0; int_y < kWindowHeight; int_y++)
    {
        float curr_y =  ((float) kWindowHeight / 2 - (float) int_y) * dy;
        float curr_x = - (float) kWindowWidth  / 2;

        for (size_t int_x = 0; int_x < kWindowWidth; int_x+= 8)
        {
            __m256 X0 = _mm256_mul_ps(_mm256_add_ps(_76543210,
                                                    _mm256_add_ps(_mm256_set1_ps((float) int_x),
                                                                  _mm256_set1_ps(        curr_x))),
                                      _mm256_set1_ps(dx));

            __m256 Y0 = _mm256_set1_ps(curr_y);

            __m256 X = X0;
            __m256 Y = Y0;

            __m256i N = _mm256_setzero_si256();

            for (int n = 0; n < kMaxIterationCount; n++)
            {
                __m256 square_x = _mm256_mul_ps(X, X);
                __m256 square_y = _mm256_mul_ps(Y, Y);

                __m256 radius_2 = _mm256_add_ps(square_x, square_y);

                __m256 cmp = _mm256_cmp_ps(radius_2, max_square_radius, _CMP_LE_OS);

                int mask = _mm256_movemask_ps(cmp);

                if (!mask)
                {
                    break;
                }

                N = _mm256_sub_epi32(N, _mm256_castps_si256(cmp));

                __m256 x_y = _mm256_mul_ps(X, Y);

                X = _mm256_add_ps(_mm256_sub_ps(square_x, square_y), X0);
                Y = _mm256_add_ps(_mm256_add_ps(x_y     , x_y     ), Y0);
            }

            int *n_pointer = (int *) &N;

            #ifdef SFML_BUFFER

                for (size_t i = 0; i < 8; i++)
                {
                    pixel.setPosition(int_x + i, int_y);

                    if (n_pointer[i] >= kMaxIterationCount)
                    {
                        pixel.setFillColor(sf::Color::Black);
                    }
                    else
                    {
                        pixel.setFillColor(sf::Color(132 - (char) n_pointer[i] * 6,
                                                    72 - (char) n_pointer[i] * 4,
                                                    178 - (char) n_pointer[i] * 9));
                    }

                    window.draw(pixel);
                }

            #else

                for (size_t i = 0; i < 8; i++)
                {
                    size_t pixel_array_pos = (int_y * kWindowWidth + int_x + i) * 4;

                    sf::Uint8 red_color   = 0;
                    sf::Uint8 green_color = 0;
                    sf::Uint8 blue_color  = 0;

                    if (n_pointer[i] < kMaxIterationCount)
                    {
                        red_color   = 132 - (char) n_pointer[i] * 6;
                        green_color = 72  - (char) n_pointer[i] * 4;
                        blue_color  = 178 - (char) n_pointer[i] * 9;
                    }

                    pixel_array[pixel_array_pos    ] = red_color;
                    pixel_array[pixel_array_pos + 1] = green_color;
                    pixel_array[pixel_array_pos + 2] = blue_color;
                    pixel_array[pixel_array_pos + 3] = 255;//what???
                }

            #endif
        }

    }
}
