#pragma once 

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <stdlib.h>
#include <math.h>
#include <immintrin.h>

#include "../logs/errors_and_logs.h"
#include "mandelbrot_conf.h"

//===============================================

#ifdef MNDLBRT_LOGS

    #define mndlbrt_log_report() log_report()

#else 

    #define mndlbrt_log_report() ""

#endif 

//===============================================

struct Mandel_struct
{
    double x_shift;
    double y_shift;

    //int x_size;
    //int y_size;

    sf::Uint32* data;

    double scale_factor;
};

//===============================================

enum Modes
{

    ONLY_CALC = 100,
    CALC_AND_SHOW = 200

};

//===============================================

union Color
{
    unsigned char RGBA[4];
    uint32_t number;
};

//===============================================

#define ARR_INT8_TO_FLOAT8(arr_int, arr_float)      \
                                                    \
    do                                              \
    {                                               \
        arr_float[0] = (float)arr_int[0];           \
        arr_float[1] = (float)arr_int[1];           \
        arr_float[2] = (float)arr_int[2];           \
        arr_float[3] = (float)arr_int[3];           \
        arr_float[4] = (float)arr_int[4];           \
        arr_float[5] = (float)arr_int[5];           \
        arr_float[6] = (float)arr_int[6];           \
        arr_float[7] = (float)arr_int[7];           \
                                                    \
    } while(0);

//---------------------------------------------------

#define ARR_FLOAT8_TO_INT8(arr_float, arr_int)      \
                                                    \
    do                                              \
    {                                               \
        arr_int[0] = (unsigned char)arr_float[0];             \
        arr_int[1] = (unsigned char)arr_float[1];             \
        arr_int[2] = (unsigned char)arr_float[2];             \
        arr_int[3] = (unsigned char)arr_float[3];             \
        arr_int[4] = (unsigned char)arr_float[4];             \
        arr_int[5] = (unsigned char)arr_float[5];             \
        arr_int[6] = (unsigned char)arr_float[6];             \
        arr_int[7] = (unsigned char)arr_float[7];             \
                                                    \
    } while(0);

//---------------------------------------------------

#define ARR_FLOAT8_SIN(arr_float)                   \
                                                    \
do                                                  \
{                                                   \
    arr_float[0] = sinf(arr_float[0]);              \
    arr_float[1] = sinf(arr_float[1]);              \
    arr_float[2] = sinf(arr_float[2]);              \
    arr_float[3] = sinf(arr_float[3]);              \
    arr_float[4] = sinf(arr_float[4]);              \
    arr_float[5] = sinf(arr_float[5]);              \
    arr_float[6] = sinf(arr_float[6]);              \
    arr_float[7] = sinf(arr_float[7]);              \
                                                    \
} while(0);

//---------------------------------------------------

#define ARR_FLOAT8_MUL_NUM(arr_float, number)       \
                                                    \
do                                                  \
{                                                   \
    arr_float[0] = number * (arr_float[0]);         \
    arr_float[1] = number * (arr_float[1]);         \
    arr_float[2] = number * (arr_float[2]);         \
    arr_float[3] = number * (arr_float[3]);         \
    arr_float[4] = number * (arr_float[4]);         \
    arr_float[5] = number * (arr_float[5]);         \
    arr_float[6] = number * (arr_float[6]);         \
    arr_float[7] = number * (arr_float[7]);         \
                                                    \
} while(0); 
//===============================================

//extern __m256  _ZGVdN8v_sinf(__m256 x); 
//extern __m256 _mm256_sin_ps(__m256 v1);

enum Modes _parse_cmndln(int argc, char* argv[] FOR_LOGS(, LOG_PARAMS));

int _mandelbrot_exec(enum Modes mode FOR_LOGS(, LOG_PARAMS));

int _mandelbrot_eval(Mandel_struct* mandel_struct FOR_LOGS(, LOG_PARAMS));

int _mandel_struct_init(Mandel_struct* mandel_struct FOR_LOGS(, LOG_PARAMS));

int _write_fps(sf::RenderWindow* window, sf::Clock* fps_clock, sf::Text* text, size_t* fps_ct FOR_LOGS(, LOG_PARAMS));

int _text_init(sf::Text* text, sf::Font* font FOR_LOGS(, LOG_PARAMS));

uint32_t _get_color(int num FOR_LOGS(, LOG_PARAMS));

//===============================================

#define get_color(num) \
       _get_color(num FOR_LOGS(LOG_ARGS))

#define text_init(text, font) \
       _text_init(text, font FOR_LOGS(, LOG_ARGS))

#define write_fps(window, clock, text, fps_ct) \
       _write_fps(window, clock, text, fps_ct FOR_LOGS(, LOG_ARGS))

#define mandel_struct_init(mandel_struct) \
       _mandel_struct_init(mandel_struct FOR_LOGS(, LOG_ARGS))

#define parse_cmndln(argc, argv) \
       _parse_cmndln(argc, argv FOR_LOGS(, LOG_ARGS))

#define mandelbrot_exec(mode) \
       _mandelbrot_exec(mode FOR_LOGS(, LOG_ARGS))

#define mandelbrot_eval(mandel_struct) \
       _mandelbrot_eval(mandel_struct FOR_LOGS(, LOG_ARGS))

// #define mandelbrot_show() \
//        _mandelbrot_show(FOR_LOGS(LOG_ARGS))
    