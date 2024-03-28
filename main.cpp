#include <stdio.h>

#include "mandelbrot.h"

const char *kWndName = "Mandelbrot";

const char *kDataFileName = "data.txt";

const char *kDataSuccessfullyObtainedMessage = "Data obtained!!!";

const size_t kMaxDataCount = 100;

int main()
{
    FILE *data_file = fopen(kDataFileName, "w");

    if (data_file == nullptr)
    {
        printf(">> Mandelbrot: Failed to open file.\n");

        return 0;
    }

    sf::RenderWindow window(sf::VideoMode(kWindowWidth, kWindowHeight), kWndName, sf::Style::Close | sf::Style::Titlebar);

    unsigned int time_count_start = 0;
    unsigned int time_count_end   = 0;

    #ifndef SFML_BUFFER

        sf::Uint8 *pixel_array = (sf::Uint8 *) calloc(kWindowWidth * kWindowHeight * 4, sizeof(sf::Uint8));

        if (pixel_array == nullptr)
        {
            printf(">> Mandelbrot: Failed to allocate memory.\n");

            window.close();

            return 0;
        }

        sf::Texture texture;
                    texture.create(kWindowWidth, kWindowHeight);
                    texture.update(pixel_array);

        sf::Sprite sprite(texture);

    #endif

    size_t obtained_data_count = 1;

    bool printed_message_state = false;

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

        time_count_start = _rdtsc();

        #ifndef SFML_BUFFER

            #ifdef AVX_IMPLEMENT

                AVX_PrintMandelbrot(window, pixel_array);

            #else

                PrintMandelbrot(window, pixel_array);

            #endif

            texture.update(pixel_array);

            window.draw(sprite);

        #else

            #ifdef AVX_IMPLEMENT

                AVX_PrintMandelbrot(window);

            #else

                PrintMandelbrot(window);

            #endif

        #endif

        time_count_end = _rdtsc();

        if (obtained_data_count <= 200 && obtained_data_count >= 101)
        {
            fprintf(data_file ,"%ld %d\n", obtained_data_count - 100,
                                           time_count_end - time_count_start);

            if (obtained_data_count == 200)
            {
                printf("%s\n", kDataSuccessfullyObtainedMessage);
            }

        }

        window.display();

        obtained_data_count++;
    }

    #ifndef SFML_BUFFER

        free(pixel_array);

    #endif

    fclose(data_file);

    return 0;
}

