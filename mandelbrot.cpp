#include <stdlib.h>
#include <math.h>

#include "mandelbrot.h"

static const size_t kMaxIterationCount = 256;

static const double kMaxRadius = 10.d;

const double ROI_x = -1.325f;
const double ROI_y = 0;

const double dx = 0.005;
const double dy = 0.005;

void PrintMandelbrot(sf::RenderWindow &window)
{
    sf::RectangleShape pixel(sf::Vector2f(1.f, 1.f));

    for (size_t int_y = 0; int_y < kWindowHeight; int_y++)
    {
        for (size_t int_x = 0; int_x < kWindowWidth; int_x++)
        {
            double curr_x = ((double) int_x - (double) kWindowWidth / 2) * dx;
            double curr_y = ((double) kWindowHeight / 2 - (double) int_y) * dy;

            size_t iteration_count = 0;

            double X = curr_x;
            double Y = curr_y;

            for (; iteration_count < kMaxIterationCount; iteration_count++)
            {
                double square_x = X * X;
                double square_y = Y * Y;
                double x_y      = X * Y;
                double radius_2 = square_x + square_y;

                if (radius_2 >= kMaxRadius)
                {
                    break;
                }

                X = square_x - square_y + curr_x;
                Y = x_y + x_y           + curr_y;
            }

            pixel.setPosition(int_x, int_y);

            if (iteration_count >= kMaxIterationCount)
            {
                pixel.setFillColor(sf::Color(0,0,0));
            }
            else
            {
                pixel.setFillColor(sf::Color(132 - (char) iteration_count * 6,
                                             72 - (char) iteration_count * 4,
                                             178 - (char) iteration_count * 9));
            }

            window.draw(pixel);
        }
    }

    window.display();
}

void DIY_AVX_PrintMandelbrot(sf::RenderWindow &window)
{
    sf::RectangleShape pixel(sf::Vector2f(1.f, 1.f));

    for (size_t int_y = 0; int_y < kWindowHeight; int_y++)
    {
        for (size_t int_x = 0; int_x < kWindowWidth; int_x++)
        {
            double curr_x = ((double) int_x - (double) kWindowWidth / 2) * dx;
            double curr_y = ((double) kWindowHeight / 2 - (double) int_y) * dy;

            size_t iteration_count = 0;

            double X = curr_x;
            double Y = curr_y;

            for (; iteration_count < kMaxIterationCount; iteration_count++)
            {
                double square_x = X * X;
                double square_y = Y * Y;
                double x_y      = X * Y;
                double radius_2 = square_x + square_y;

                if (radius_2 >= kMaxRadius)
                {
                    break;
                }

                X = square_x - square_y + curr_x;
                Y = x_y + x_y           + curr_y;
            }

            pixel.setPosition(int_x, int_y);

            if (iteration_count >= kMaxIterationCount)
            {
                pixel.setFillColor(sf::Color(0,0,0));
            }
            else
            {
                pixel.setFillColor(sf::Color(132 - (char) iteration_count * 6,
                                             72 - (char) iteration_count * 4,
                                             178 - (char) iteration_count * 9));
            }

            window.draw(pixel);
        }
    }

    window.display();
}

void AVX_PrintMandelbrot(sf::RenderWindow &window)
{
    sf::RectangleShape pixel(sf::Vector2f(1.f, 1.f));

    for (size_t int_y = 0; int_y < kWindowHeight; int_y++)
    {
        for (size_t int_x = 0; int_x < kWindowWidth; int_x++)
        {
            double curr_x = ((double) int_x - (double) kWindowWidth / 2) * dx;
            double curr_y = ((double) kWindowHeight / 2 - (double) int_y) * dy;

            size_t iteration_count = 0;

            double X = curr_x;
            double Y = curr_y;

            for (; iteration_count < kMaxIterationCount; iteration_count++)
            {
                double square_x = X * X;
                double square_y = Y * Y;
                double x_y      = X * Y;
                double radius_2 = square_x + square_y;

                if (radius_2 >= kMaxRadius)
                {
                    break;
                }

                X = square_x - square_y + curr_x;
                Y = x_y + x_y           + curr_y;
            }

            pixel.setPosition(int_x, int_y);

            if (iteration_count >= kMaxIterationCount)
            {
                pixel.setFillColor(sf::Color(0,0,0));
            }
            else
            {
                pixel.setFillColor(sf::Color(132 - (char) iteration_count * 6,
                                             72 - (char) iteration_count * 4,
                                             178 - (char) iteration_count * 9));
            }

            window.draw(pixel);
        }
    }

    window.display();
}
