#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <stdlib.h>
#include <math.h>

#include <immintrin.h>

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

    mandel_struct->x_shift = Init_x_shift;
    mandel_struct->y_shift = Init_y_shift;

    mandel_struct->scale_factor = Init_scale_factor;

    return 0;
}

//-----------------------------------------------

int _mandelbrot_exec(enum Modes mode FOR_LOGS(, LOG_PARAMS))
{
    mndlbrt_log_report();

    char no_evaluation_flag = 0;
    char no_evaluation_opt  = 0;

    Mandel_struct mandel_struct = { 0 };
    mandel_struct_init(&mandel_struct);

    sf::RenderWindow window(sf::VideoMode(X_SIZE, Y_SIZE), "Mandelbrodina");

    sf::Texture mandel_texture;
    mandel_texture.create(X_SIZE, Y_SIZE);

    mandel_struct.data = new sf::Uint32[X_SIZE * Y_SIZE];
    mandel_texture.update((sf::Uint8*) mandel_struct.data, X_SIZE, Y_SIZE, 0, 0);

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
                if (event.key.code == sf::Keyboard::O)
                {
                    mandel_struct.scale_factor += Scale_step * mandel_struct.scale_factor;
                    no_evaluation_flag = 0;
                }

                else if (event.key.code == sf::Keyboard::P)
                { 
                    mandel_struct.scale_factor -= Scale_step * mandel_struct.scale_factor;
                    no_evaluation_flag = 0;
                }

                else if (event.key.code == sf::Keyboard::Up)
                {
                    mandel_struct.y_shift += Y_shift_step * mandel_struct.scale_factor;
                    //printf("\n y+ %lf step*scale %lf\n", mandel_struct.y_shift, Y_shift_step * mandel_struct.scale_factor);

                    no_evaluation_flag = 0;
                }

                else if (event.key.code == sf::Keyboard::Down)
                {
                    mandel_struct.y_shift -= Y_shift_step * mandel_struct.scale_factor;
                    //printf("\n y- %lf step*scale %lf\n", mandel_struct.y_shift, Y_shift_step * mandel_struct.scale_factor);
                    no_evaluation_flag = 0;
                }

                else if (event.key.code == sf::Keyboard::Right)
                {
                    mandel_struct.x_shift -= X_shift_step * mandel_struct.scale_factor;
                    //printf("\n x- %lf step*scale %lf\n", mandel_struct.x_shift, X_shift_step * mandel_struct.scale_factor);
                    no_evaluation_flag = 0;
                }

                else if (event.key.code == sf::Keyboard::Left)
                {
                    mandel_struct.x_shift += X_shift_step * mandel_struct.scale_factor;
                    //printf("\n x+ %lf step*scale %lf\n", mandel_struct.x_shift, X_shift_step * mandel_struct.scale_factor);
                    no_evaluation_flag = 0;
                }

                else if (event.key.code == sf::Keyboard::LAlt)
                {
                    if (!no_evaluation_opt)
                        no_evaluation_opt = 1;
                    else
                        no_evaluation_opt = 0;
                }
            }
        }

        window.clear();

        //printf("\n flag is %d \n", no_evaluation_flag);

        if (no_evaluation_opt && no_evaluation_flag)
        {
            window.draw(sprite);
        }

        else
        {
            if (mandelbrot_eval(&mandel_struct) == -1)
            {
                error_report(MANDEL_EVAL_ERR);
                return -1;
            }

            if (mode != ONLY_CALC)
            {
                mandel_texture.update((sf::Uint8*) mandel_struct.data, X_SIZE, Y_SIZE, 0, 0);
                window.draw(sprite);
            }
        }

        if (write_fps(&window, &fps_clock, &fps_text, &fps_ct) == -1)
            return -1;

        window.display();
        no_evaluation_flag = 1;
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

    if (*fps_ct == SIZE_MAX || cur_time > 1.f)
    {
        (*fps_clock).restart();
        *fps_ct = 0;
    }

    char fps_buf[Fps_buf_size] = { 0 };
    sprintf(fps_buf, "%04.2f", fps);

    (*text).setString(fps_buf);
    (*window).draw(*text);

    return 0;
}

//-----------------------------------------------

int _mandelbrot_eval(Mandel_struct* mandel_struct FOR_LOGS(, LOG_PARAMS))
{
    mndlbrt_log_report();

    double x_shift = mandel_struct->x_shift;
    double y_shift = mandel_struct->y_shift;
    double scale_factor = mandel_struct->scale_factor;

    for (int y_ct = 0; y_ct < Y_SIZE; y_ct++)
    {
        double x0 = (            - X_SIZE/2) * Mul_x * scale_factor + x_shift * Mul_x;
        double y0 = ((float)y_ct - Y_SIZE/2) * Mul_y * scale_factor + y_shift * Mul_y;

        double x0_dif = Mul_x * scale_factor;
        for (int x_ct = 0; x_ct < X_SIZE; x_ct++, x0 += x0_dif)
        {

            double x = x0; 
            double y = y0;

            int num = 0;
            for (; num < Check_num; num++)
            {
                double x2 = x * x;
                double y2 = y * y;
                double xy = x * y;

                double r2 = x2 + y2;

                if (r2 >= Max_r2)
                    break;

                x = x2 - y2   + x0;
                y = 2  * xy   + y0;
            }

            mandel_struct->data[x_ct + y_ct * X_SIZE] =(num < Check_num)? get_color(num) : 1;
        }
    }

    return 0;
}

//-----------------------------------------------

uint32_t _get_color(int num FOR_LOGS(, LOG_PARAMS))
{
    mndlbrt_log_report();

    Color color = { 0 };

    num += 1 - log(log2f(abs(num)));

    // if ((num % 2) == 1) 
    //     color.number = 0;
    // else 
    //     color.number = 0xFFFFFFFF;

    // if (num >= Check_num)
    //     color.number = 0;

    color.RGBA[0] = (unsigned char)(180 - num - (num % 2) * 120);
    color.RGBA[1] = (unsigned char)(10 + 25 * ((num + 1) % 2));
    color.RGBA[2] = (unsigned char)((num + 1) * 11);
    color.RGBA[3] = 0xCF;

    // color.RGBA[0] = (unsigned char)(255 - num);
    // color.RGBA[1] = (unsigned char)((num % 2) * 64);
    // color.RGBA[2] = (unsigned char)(num);
    // color.RGBA[3] = 0xCF;

    return color.number;
}
