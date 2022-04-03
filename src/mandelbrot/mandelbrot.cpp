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
                if (event.key.code == sf::Keyboard::PageDown)
                {
                    mandel_struct.scale_factor += Scale_step * mandel_struct.scale_factor;
                    no_evaluation_flag = 0;
                }

                else if (event.key.code == sf::Keyboard::PageUp)
                { 
                    mandel_struct.scale_factor -= Scale_step * mandel_struct.scale_factor;
                    no_evaluation_flag = 0;
                }

                else if (event.key.code == sf::Keyboard::Down)
                {
                    mandel_struct.y_shift += Y_shift_step * mandel_struct.scale_factor;
                    no_evaluation_flag = 0;
                }

                else if (event.key.code == sf::Keyboard::Up)
                {
                    mandel_struct.y_shift -= Y_shift_step * mandel_struct.scale_factor;
                    no_evaluation_flag = 0;
                }

                else if (event.key.code == sf::Keyboard::Left)
                {
                    mandel_struct.x_shift -= X_shift_step * mandel_struct.scale_factor;
                    no_evaluation_flag = 0;
                }

                else if (event.key.code == sf::Keyboard::Right)
                {
                    mandel_struct.x_shift += X_shift_step * mandel_struct.scale_factor;
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

    float scale_mul_x = Mul_x * scale_factor;

    const __m256  _max_r2      = _mm256_set1_ps(Max_r2);
    const __m256  _scale_mul_x = _mm256_set1_ps(scale_mul_x);
    const __m256i _check_num   = _mm256_set1_epi32(Check_num);
    const __m256  _iter        = _mm256_set_ps(7, 6, 5, 4, 3, 2, 1, 0);

    for (int y_ct = 0; y_ct < Y_SIZE; y_ct++)
    {
        float x0 = (            - X_SIZE/2) * scale_mul_x          + x_shift * Mul_x;
        float y0 = ((float)y_ct - Y_SIZE/2) * Mul_y * scale_factor + y_shift * Mul_y; 

        float x0_dif  = 8 * scale_mul_x;

        for (int x_ct = 0; x_ct < X_SIZE; x_ct += 8, x0 += x0_dif)
        {
            __m256 _x0 = _mm256_add_ps (_mm256_set1_ps(x0), _mm256_mul_ps(_iter, _scale_mul_x));
            __m256 _y0 = _mm256_set1_ps(y0);
            

            __m256 _x = _x0;
            __m256 _y = _y0;

            __m256i _num = _mm256_setzero_si256();

            for (int num = 0; num < Check_num; num++)
            {
                __m256 _x2  = _mm256_mul_ps(_x, _x);
                __m256 _y2  = _mm256_mul_ps(_y, _y);

                __m256 _r2  = _mm256_add_ps(_x2, _y2);

                __m256 _cmp = _mm256_cmp_ps(_r2, _max_r2, _CMP_LE_OQ);

                int mask = _mm256_movemask_ps(_cmp);
                if (!mask) break;

                _num = _mm256_sub_epi32(_num, _mm256_castps_si256(_cmp));

                __m256 _xy = _mm256_mul_ps(_x, _y);
                _x = _mm256_add_ps(_mm256_sub_ps(_x2, _y2), _x0);
                _y = _mm256_add_ps(_mm256_add_ps(_xy, _xy), _y0);
            }

            alignas(32) int   numbers  [8] = { 0 };
            alignas(32) float numbers_f[8] = { 0 }; 
            _mm256_maskstore_epi32(numbers, _mm256_set1_epi8(0xFF), _num);

            ARR_INT8_TO_FLOAT8(numbers, numbers_f);

            __m256 _num_f = _mm256_load_ps(numbers_f);
            __m256 _256_f = _mm256_set1_ps(256.f);
                   _num_f = _mm256_div_ps (_num_f, _256_f);

            __m256 _pi_f  = _mm256_set1_ps(3.141592f);
                  _num_f  = _mm256_mul_ps(_num_f, _pi_f);

            //_num_f = _ZGVdN8v_sinf(_num_f);
            //_num_f = _mm256_sin_ps(_num_f);
            //_num_f = _mm256_mul_ps(_num_f, _256_f);

            float color_values_float[8] = { 0 };
            int color_values_int[8] = { 0 };

            _mm256_maskstore_ps(color_values_float, _mm256_set1_epi8(0xFF), _num_f);

            ARR_FLOAT8_SIN(color_values_float);
            ARR_FLOAT8_MUL_NUM(color_values_float, 256.f);
            ARR_FLOAT8_TO_INT8(color_values_float, color_values_int);

            __m256i _colors_int = _mm256_load_si256((__m256i*)color_values_int);
            __m256i _256_int    = _mm256_set1_epi32(256);
            __m256i _255_int    = _mm256_set1_epi32(255);  
            __m256i _256q_int   = _mm256_set1_epi32(256*256);

            __m256i _temp_eval  = _mm256_add_epi32(_mm256_mul_epi32(_256_int, _colors_int), _255_int);
            __m256i _colors     = _mm256_add_epi32(_mm256_mul_epi32(_256q_int, _temp_eval), _colors_int);

            __m256i _colors256  = _mm256_mul_epi32(_256_int, _colors_int);
            _colors             = _mm256_add_epi32(_colors, _colors256);

            alignas(32) int colors[8] = { 0 };
            _mm256_maskstore_epi32(colors, _mm256_set1_epi8(0xFF), _colors);

            int offset = x_ct + y_ct * X_SIZE;

            mandel_struct->data[offset + 0] = colors[0];
            mandel_struct->data[offset + 1] = colors[1];
            mandel_struct->data[offset + 2] = colors[2];
            mandel_struct->data[offset + 3] = colors[3];
            mandel_struct->data[offset + 4] = colors[4];
            mandel_struct->data[offset + 5] = colors[5];
            mandel_struct->data[offset + 6] = colors[6];
            mandel_struct->data[offset + 7] = colors[7];
        }
    }
    
    return 0;
}

//-----------------------------------------------

uint32_t _get_color(int num FOR_LOGS(, LOG_PARAMS))
{ 
    mndlbrt_log_report(); 
 
    Color color = { 0 }; 
 
    float num_f = (float)num; 
 
    if (num >= Check_num) 
        return 0xFFFFFFFF; 
 
    unsigned char color_byte  = (unsigned char) (sin (num_f / 256 * 3.14) * 255);
  
    color.RGBA[0] = color_byte; 
    color.RGBA[1] = color_byte; 
    color.RGBA[2] = 255; 
    color.RGBA[3] = color_byte; 
  
    // color.RGBA[0] = num_f; 
    // color.RGBA[1] = num_f; 
    // color.RGBA[2] = 255; 
    // color.RGBA[3] = num_f; 
 
    return color.number; 
} 
