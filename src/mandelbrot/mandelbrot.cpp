#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "mandelbrot.h"

//===============================================

enum Modes _parse_cmndln(int argc, char* argv[] FOR_LOGS(, LOG_PARAMS))
{
    mndlbrt_log_report();

    return CALC_AND_SHOW;
}

//-----------------------------------------------

int _mandel_struct_init(Mandel_struct* mandel_struct FOR_LOGS(, LOG_PARAMS))
{
    mndlbrt_log_report();
    assert(mandel_struct);

    mandel_struct->x_size = INIT_X_SIZE;
    mandel_struct->y_size = INIT_Y_SIZE;

    mandel_struct->scale_factor = INIT_SCALE_FACTOR;

    return 0;
}

//-----------------------------------------------

int _mandelbrot_exec(enum Modes mode FOR_LOGS(, LOG_PARAMS))
{
    mndlbrt_log_report();

    Mandel_struct mandel_struct = { 0 };
    mandel_struct_init(&mandel_struct);

    sf::RenderWindow window(sf::VideoMode(x_size, y_size), "Mandelbrodina");

    sf::Texture mandel_texture;
    mandel_texture.create(x_size, y_size);

    sf::Uint32* mandel_data = new sf::Uint32[x_size * y_size];
    mandel_texture.update((sf::Uint8*) mandel_data, x_size, y_size, 0, 0);

    sf::Sprite sprite;
    sprite.setTexture(mandel_texture);

    //unsigned int fps_ct = 0;
    //sf::Clock fps_clock;

    while (window.isOpen())
    {

        if (mandelbrot_eval())

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::RShift)
                {
                    scale_factor = 
                }

                if (event.key.code == sf::Keyboard::LShift)
                {

                }

                if (event.key.code == sf::Keyboard::Up)
                {

                }

                if (event.key.code == sf::Keyboard::Down)
                {
                    
                }

                if (event.key.code == sf::Keyboard::Right)
                {
                    
                }

                if (event.key.code == sf::Keyboard::Left)
                {
                    
                }
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();

    }

    // if (mandelbrot_eval() == -1)
    //     return -1;

    // if (mode == CALC_AND_SHOW)
    // {
    //     if (mandelbrot_show() == -1)
    //         return -1;
    // }
    // else if (mode != ONLY_CALC)
    // {
    //     error_report(INV_MANDEL_MODE);
    //     return -1;
    // }

    return 0;
}

//-----------------------------------------------

int _mandelbrot_eval(FOR_LOGS(LOG_PARAMS))
{
    mndlbrt_log_report();

    #ifdef OPT

        // 

    #else 

        //

    #endif 

    return 0;
}

//-----------------------------------------------

int _mandelbrot_show(FOR_LOGS(LOG_PARAMS))
{
    mndlbrt_log_report();

    return 0;
}