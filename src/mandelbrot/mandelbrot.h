#pragma once 

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

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
    