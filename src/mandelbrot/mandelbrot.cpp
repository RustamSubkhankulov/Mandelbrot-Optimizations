#include <SFML/System.hpp>

#include "mandelbrot.h"

//===============================================

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

int _mandelbrot_exec(FOR_LOGS(LOG_PARAMS))
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

    sf::Clock eval_clock;

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
                }

                else if (event.key.code == sf::Keyboard::PageUp)
                { 
                    mandel_struct.scale_factor -= Scale_step * mandel_struct.scale_factor;
                }

                else if (event.key.code == sf::Keyboard::Down)
                {
                    mandel_struct.y_shift += Y_shift_step * mandel_struct.scale_factor;
                }

                else if (event.key.code == sf::Keyboard::Up)
                {
                    mandel_struct.y_shift -= Y_shift_step * mandel_struct.scale_factor;
                }

                else if (event.key.code == sf::Keyboard::Left)
                {
                    mandel_struct.x_shift -= X_shift_step * mandel_struct.scale_factor;
                }

                else if (event.key.code == sf::Keyboard::Right)
                {
                    mandel_struct.x_shift += X_shift_step * mandel_struct.scale_factor;
                }
            }
        }

        #ifndef NO_DRAWING

            window.clear();
            window.draw(sprite);

        #endif 

        int return_value = mandelbrot_eval(&mandel_struct);
        if (return_value == -1)
        {
            error_report(MANDEL_EVAL_ERR);
            return -1;
        }

        #ifndef NO_DRAWING

            mandel_texture.update((sf::Uint8*) mandel_struct.data, X_SIZE, Y_SIZE, 0, 0);
            window.draw(sprite);

        #endif 
            
        return_value = write_fps(&window, &fps_clock, &fps_text, &fps_ct);
        if (return_value == -1)
            return -1;

        #ifndef NO_DRAWING

            window.display();

        #endif 

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

    #ifndef NO_DRAWING

        char fps_buf[Fps_buf_size] = { 0 };
        sprintf(fps_buf, "%04.2f", fps);

        (*text).setString(fps_buf);
        (*window).draw(*text);

    #else 

        printf("\n FPS: %04.2f \n", fps);

    #endif 

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
            }                                                   // next code is setting up colors for each pixel

            alignas(32) int   numbers  [8] = { 0 };
            alignas(32) float numbers_f[8] = { 0 }; 
            _mm256_storeu_si256((__m256i*)numbers, _num);

            ARR_INT8_TO_FLOAT8(numbers, numbers_f);             // convert vector of int values
                                                                // into vector of floats
            __m256 _num_f = _mm256_load_ps(numbers_f);
            __m256 _255_f = _mm256_set1_ps(255.f);
                   _num_f = _mm256_div_ps (_num_f, _255_f);    
                                                        
            __m256 _pi_f  = _mm256_set1_ps(3.141592f);          // perform some arithmetics
                  _num_f  = _mm256_mul_ps(_num_f, _pi_f);

            float color_values_float[8] = { 0 };
            int   color_values_int[8]   = { 0 };               

            _mm256_store_ps(color_values_float, _num_f);        // store vector of float values

            ARR_FLOAT8_SIN(color_values_float);                 // calculate sine from each value
            ARR_FLOAT8_MUL_NUM(color_values_float, 255.f);      // multiply each by 255
            ARR_FLOAT8_TO_INT8(color_values_float, color_values_int);
                                                                // convert to array of ints and store in vector
            __m256i _colors_int = _mm256_load_si256((__m256i*)color_values_int);
            
            __m256i _255_int    = _mm256_set1_epi32(255);    

            __m256i _temp_eval  = _mm256_add_epi32(_mm256_slli_epi32(_colors_int, 8), _255_int);
            __m256i _colors     = _mm256_add_epi32(_mm256_slli_epi32(_temp_eval, 16), _colors_int);

            __m256i _colors256  = _mm256_slli_epi32(_colors_int, 8);
            _colors             = _mm256_add_epi32(_colors, _colors256);

            int offset = x_ct + y_ct * X_SIZE;
            _mm256_storeu_si256((__m256i*)(&mandel_struct->data[offset]), _colors);
                                                                // store calculated colors in data array
        }
    }
    
    return 0;
}
