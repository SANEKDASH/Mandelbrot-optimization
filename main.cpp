#include "mandelbrot.h"

const char *kWndName = "Mandelbrot";

int main()
{
    sf::RenderWindow window(sf::VideoMode(kWindowWidth, kWindowHeight), kWndName, sf::Style::Close | sf::Style::Titlebar);

    while(window.isOpen())
    {
        sf::Event evnt;

        while (window.pollEvent(evnt))
        {
            if (evnt.type == evnt.Closed)
            {
                window.close();
            }
        }

        sf::Clock clock;

        PrintMandelbrot(window);

        float render_time = clock.getElapsedTime().asSeconds();

        printf("Time to print: %f\n", render_time);

    }

    return 0;
}

