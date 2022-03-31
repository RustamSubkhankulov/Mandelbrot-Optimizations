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
                }

                if (event.key.code == sf::Keyboard::P)
                { 
                    mandel_struct.scale_factor -= Scale_step * mandel_struct.scale_factor;
                }

                if (event.key.code == sf::Keyboard::Up)
                {
                    mandel_struct.y_shift += Y_shift_step * mandel_struct.scale_factor;
                }

                if (event.key.code == sf::Keyboard::Down)
                {
                    mandel_struct.y_shift -= Y_shift_step * mandel_struct.scale_factor;
                }

                if (event.key.code == sf::Keyboard::Right)
                {
                    mandel_struct.x_shift -= X_shift_step * mandel_struct.scale_factor;
                }

                if (event.key.code == sf::Keyboard::Left)
                {
                    mandel_struct.x_shift += X_shift_step * mandel_struct.scale_factor;
                }
            }
        }

        window.clear();

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

    int x_shift = mandel_struct->x_shift;
    int y_shift = mandel_struct->y_shift;
    float scale_factor = mandel_struct->scale_factor;

    //printf("\n scale %f x shift %d y shift %d \n", scale_factor, x_shift, y_shift);

//---------------------------------------------------------------------------------------

#ifdef OPT

    float scale_mul_x = Mul_x * scale_factor;

    const __m256  _max_r2      = _mm256_set1_ps(Max_r2);
    const __m256  _scale_mul_x = _mm256_set1_ps(scale_mul_x);
    const __m256i _check_num   = _mm256_set1_epi32(Check_num);
    const __m256i _iter        = _mm256_set1_epi32(8, 7, 6, 5, 4, 3, 2, 1, 0);

    for (int y_ct = 0; y_ct < Y_SIZE; y_ct++)
    {
        float x0 = (            - X_SIZE/2) * scale_mul_x          + x_shift * Mul_x;
        float y0 = ((float)y_ct - Y_SIZE/2) * Mul_y * scale_factor + y_shift * Mul_y; 

        float x0_dif  = 8 * scale_mul_x;

        for (int x_ct = 0; x_ct < X_SIZE; x_ct += 8, x0 += x0_dif)
        {
            __m256 _x0 = _mm256_add_ps (_mm256_set1_ps(x0), _mm256_mul_ps(_iter, _scale_mul_x));
            __m256 _y0 = _mm256_set1_ps(y0);
        }

        __m256 _x = _x0;
        __m256 _y = _y0;

        __m256 _num = _mm256_setzero_si256();

        for (int num = 0; num < Check_num; num++)
        {
            __m256 _x2  = _mm256_mul_ps(_x, _x);
            __m256 _y2  = _mm256_mul_ps(_y, _y);

            __m256 _r2  = _mm256_add_ps(_x2, _y2);

            __m256 _cmp = _mm256_cmp_ps(_r2, _max_r2, _CMP_LE_OQ);

            int mask = _mm256_movemask_ps(_cmp);
            if (!mask) break;

            _num = _mm256_sub_epi32(_num, _mm_castps_si256(cmp));

            __m256 _xy = _mm256_mul_ps(_x, _y);
            _x = _mm256_add_ps(_mm256_sub_ps(_x2, _y2), _x0);
            _y = _mm256_add_ps(_mm256_add_ps(_xy, _xy), _y0);
        }

        
    }

//---------------------------------------------------------------------------------------

#else 

    for (int y_ct = 0; y_ct < Y_SIZE; y_ct++)
    {
        float x0 = (            - X_SIZE/2) * Mul_x * scale_factor + x_shift * Mul_x;
        float y0 = ((float)y_ct - Y_SIZE/2) * Mul_y * scale_factor + y_shift * Mul_y;    

        float x0_dif = Mul_x * scale_factor;
        for (int x_ct = 0; x_ct < X_SIZE; x_ct++, x0 += x0_dif)
        {

            float x = x0; 
            float y = y0;

            int num = 0;
            for (; num < Check_num; num++)
            {
                float x2 = x*x;
                float y2 = y*y;

                float r2 = x2 + y2;

                if (r2 >= Max_r2)
                    break;

                x = x2 - y2   + x0;
                y = 2 * x * y + y0;
            }

            mandel_struct->data[x_ct + y_ct * X_SIZE] =(num < Check_num)? get_color(num) : 1;
        }
    }

#endif 

    return 0;
}

//-----------------------------------------------

uint32_t _get_color(int num FOR_LOGS(, LOG_PARAMS))
{
    mndlbrt_log_report();

    Color color = { 0 };

    num += 1 - log(log2f(abs(num)));

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
