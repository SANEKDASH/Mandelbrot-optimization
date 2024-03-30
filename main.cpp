#include <stdio.h>

#include "mandelbrot.h"

const char *kWndName = "Mandelbrot";

const char *kDataFileName = "data.txt";

const char *kDataSuccessfullyObtainedMessage = "\e[1;32m Data obtained!!! \e[0m";

const size_t kPixelStep = 10;

const float kScaleMultiplier = 1.1;

const size_t kMaxDataCount = 100;

int main()
{
    SPEED_TEST_ONLY(FILE *data_file = fopen(kDataFileName, "w");

                    if (data_file == nullptr)
                    {
                        printf(">> Mandelbrot: Failed to open file.\n");

                        return 0;
                    })

    sf::RenderWindow window(sf::VideoMode(kWindowWidth, kWindowHeight), kWndName, sf::Style::Close | sf::Style::Titlebar);

    SPEED_TEST_ONLY(unsigned int time_count_start = 0;
                    unsigned int time_count_end   = 0;)

    #ifndef SFML_BUFFER

        sf::Uint8 *pixel_array = (sf::Uint8 *) calloc(kWindowWidth * kWindowHeight * kColorAttrNumber, sizeof(sf::Uint8));

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

    SPEED_TEST_ONLY(size_t obtained_data_count = 1;

                    bool printed_message_state = false;)

    ViewProperties view_properties = {1, 0, 0};

    while(window.isOpen())
    {
        sf::Event evnt;

        while (window.pollEvent(evnt))
        {
            if (evnt.type == evnt.Closed)
            {
                window.close();
            }
            else if (evnt.type == evnt.KeyPressed)
            {
                switch (evnt.key.code)
                {
                    case sf::Keyboard::Up:
                    {
                        view_properties.y_shift += kPixelStep;

                        break;
                    }

                    case sf::Keyboard::Down:
                    {
                        view_properties.y_shift -= kPixelStep;

                        break;
                    }

                    case sf::Keyboard::Left:
                    {
                        view_properties.x_shift -= kPixelStep;

                        break;
                    }

                    case sf::Keyboard::Right:
                    {
                        view_properties.x_shift += kPixelStep;

                        break;
                    }

                    case sf::Keyboard::Dash:
                    {
                        view_properties.scale /= kScaleMultiplier;

                        break;
                    }

                    case sf::Keyboard::Equal:
                    {
                        view_properties.scale *= kScaleMultiplier;

                        break;
                    }

                    default:
                    {
                        break;
                    }
                }
            }
        }

        SPEED_TEST_ONLY(time_count_start = _rdtsc();)

        #ifndef SFML_BUFFER

            #ifdef AVX_IMPLEMENT

                AVX_PrintMandelbrot(window, pixel_array, &view_properties);

            #else

                PrintMandelbrot(window, pixel_array, &view_properties);

            #endif

            texture.update(pixel_array);

            window.draw(sprite);

        #else

            #ifdef AVX_IMPLEMENT

                AVX_PrintMandelbrot(window, nullptr, &view_properties);

            #else

                PrintMandelbrot(window, nullptr, &view_properties);

            #endif

        #endif

        SPEED_TEST_ONLY(time_count_end = _rdtsc();

                        if (obtained_data_count <= 200 && obtained_data_count >= 101)
                        {
                            fprintf(data_file ,"%ld %d\n", obtained_data_count - 100,
                                                           time_count_end - time_count_start);

                            if (obtained_data_count == 200)
                            {
                                printf("%s\n", kDataSuccessfullyObtainedMessage);

                                fclose(data_file);
                            }
                        }

                        obtained_data_count++;)

        window.display();

    }

    #ifndef SFML_BUFFER

        free(pixel_array);

    #endif

    return 0;
}

