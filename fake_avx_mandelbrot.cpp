#include "mandelbrot.h"

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

//==================================================================================

static inline void mm_set_psl(float mm[kVectorSize],
                              float val)
{
    for (size_t i = 0; i < kVectorSize; i++)
    {
        mm[i] = val;
    }
}

//==================================================================================

static inline void mm_cpy_ps(float mm[kVectorSize],
                             float mm2[kVectorSize])
{
    for (size_t i = 0; i < kVectorSize; i++)
    {
        mm[i] = mm2[i];
    }
}

//==================================================================================

static inline void mm_add_ps(      float mm[kVectorSize],
                             const float mm1[kVectorSize],
                             const float mm2[kVectorSize])
{
    for (size_t i = 0; i < kVectorSize; i++)
    {
        mm[i] = mm1[i] + mm2[i];
    }
}

//==================================================================================

static inline void mm_sub_ps(      float mm[kVectorSize],
                             const float mm1[kVectorSize],
                             const float mm2[kVectorSize])
{
    for (size_t i = 0; i < kVectorSize; i++)
    {
        mm[i] = mm1[i] - mm2[i];
    }
}

//==================================================================================

static inline void mm_mul_ps(      float mm[kVectorSize],
                             const float mm1[kVectorSize],
                             const float mm2[kVectorSize])
{
    for (size_t i = 0; i < kVectorSize; i++)
    {
        mm[i] = mm1[i] * mm2[i];
    }
}

//==================================================================================

static inline void mm_add_epi32(      size_t mm[kVectorSize],
                                const size_t mm1[kVectorSize],
                                const float mm2[kVectorSize])
{
    for (size_t i = 0; i < kVectorSize; i++)
    {
        mm[i] = mm1[i] + !!mm2[i];
    }
}

//==================================================================================

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

//==================================================================================

static inline int mm_movemask_ps(const float cmp[kVectorSize])
{
    int mask = 0;

    for (size_t i = 0; i < kVectorSize; i++)
    {
        mask |= (!!cmp[i] << i);
    }

    return mask;
}

//==================================================================================

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

        mm_set_psl(curr_y, ((float) kWindowHeight / 2 - (float) int_y) * kDevY);

        for (size_t int_x = 0; int_x < kWindowWidth; int_x += kVectorSize)
        {
            float curr_x[kVectorSize] = {};

            // #define (...)
            mm_set_ps(curr_x, ((float) int_x + 0  - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 1  - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 2  - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 3  - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 4  - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 5  - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 6  - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 7  - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 8  - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 9  - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 10 - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 11 - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 12 - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 13 - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 14 - (float) kWindowWidth / 2) * kDevX,
                              ((float) int_x + 15 - (float) kWindowWidth / 2) * kDevX);

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

                float x_y[kVectorSize] = {}; mm_mul_ps(x_y, X, Y);

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
