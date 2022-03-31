#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <stdlib.h>

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

    //mandel_struct->x_size = Init_x_size;
    //mandel_struct->y_size = Init_y_size;
    mandel_struct->x_shift = 0;
    mandel_struct->y_shift = 0;

    mandel_struct->scale_factor = Init_scale_factor;

    return 0;
}

//-----------------------------------------------

int _mandelbrot_exec(enum Modes mode FOR_LOGS(, LOG_PARAMS))
{
    mndlbrt_log_report();

    Mandel_struct mandel_struct = { 0 };
    mandel_struct_init(&mandel_struct);

    sf::RenderWindow window(sf::VideoMode(mandel_struct.x_size, mandel_struct.y_size), "Mandelbrodina");

    sf::Texture mandel_texture;
    mandel_texture.create(mandel_struct.x_size, mandel_struct.y_size);

    mandel_struct.data = new sf::Uint32[mandel_struct.x_size * mandel_struct.y_size];
    mandel_texture.update((sf::Uint8*) mandel_struct.data, mandel_struct.x_size, mandel_struct.y_size, 0, 0);

    sf::Sprite sprite;
    sprite.setTexture(mandel_texture);

    size_t fps_ct = 0;
    sf::Clock fps_clock;

    sf::Font fps_font;
    sf::Text fps_text;
    if (text_init(&fps_text, &fps_font) == -1)
        return -1;

    while (window.isOpen())
    {
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
                    mandel_struct.scale_factor += Scale_step;
                }

                if (event.key.code == sf::Keyboard::LShift)
                {
                    mandel_struct.scale_factor -= Scale_step;
                }

                if (event.key.code == sf::Keyboard::Up)
                {
                    mandel_struct.y_shift += Y_shift_step;
                }

                if (event.key.code == sf::Keyboard::Down)
                {
                    mandel_struct.y_shift -= Y_shift_step;
                }

                if (event.key.code == sf::Keyboard::Right)
                {
                    mandel_struct.x_shift += X_shift_step;
                }

                if (event.key.code == sf::Keyboard::Left)
                {
                    mandel_struct.x_shift -= X_shift_step;
                }
            }
        }

        //
        //float sleep_time = (float)( rand() % 100) / 1000;
        //sf::sleep(sf::seconds(sleep_time));
        //sf::sleep(sf::seconds(0.016f));
        //

        if (mandelbrot_eval(mandel_struct) == -1)
        {
            error_report(MANDEL_EVAL_ERR);
            return -1;
        }

        window.clear();

        if (mode != ONLY_CALC)
            window.draw(sprite);

        if (write_fps(&window, &fps_clock, &fps_text, &fps_ct) == -1)
            return -1;

        window.display();
    }

    delete[] mandel_struct.data;

    return 0;
}

//-----------------------------------------------

int _text_init(sf::Text* text, sf::Font* font FOR_LOGS(, LOG_PARAMS))
{
    mndlbrt_log_report();
    assert(text);
    assert(font);

    (*font).loadFromFile(Font_filename);

    (*text).setFont(*font);
    (*text).setCharacterSize(64);
    (*text).setFillColor(Fps_color);

    return 0;
}

//-----------------------------------------------

int _write_fps(sf::RenderWindow* window, sf::Clock* fps_clock, sf::Text* text, size_t* fps_ct   
                                                                      FOR_LOGS(, LOG_PARAMS))
{
    mndlbrt_log_report();

    if (!fps_clock)
    {
        error_report(INV_CLOCK_PTR);
        return -1;
    }
    *fps_ct += 1;

    float cur_time = (*fps_clock).getElapsedTime().asSeconds();
    float fps = *fps_ct / (cur_time);

    if (*fps_ct == SIZE_MAX)
    {
        (*fps_clock).restart();
        *fps_ct = 0;
    }

    printf("\n frame ct : %ld time %f \n", *fps_ct, cur_time);

    char fps_buf[Fps_buf_size] = { 0 };
    sprintf(fps_buf, "%04.2f", fps);

    (*text).setString(fps_buf);
    (*window).draw(*text);

    return 0;
}

//-----------------------------------------------

int _mandelbrot_eval(Mandel_struct mandel_struct FOR_LOGS(, LOG_PARAMS))
{
    mndlbrt_log_report();

#ifdef OPT

        // 

#else 

    for (int y_ct = 0; y_ct < 600; y_ct++)
    {
        float x0 = (0 - X_SIZE/2) 
    }

#endif 

    return 0;
}

//-----------------------------------------------

int _mandelbrot_show(FOR_LOGS(LOG_PARAMS))
{
    mndlbrt_log_report();

    return 0;
}