#pragma once 

#include "../logs/errors_and_logs.h"
#include "mandelbrot_conf.h"

//===============================================

#ifdef MNDLBRT_LOGS

    #define mndlbrt_log_report() log_report()

#else 

    #define mndlbrt_log_report() ""

#endif 

//===============================================

Mandel_struct
{
    int x_shift = 0;
    int y_shift = 0;

    sf::Uint32* data;

    double scale_factor;
}

//===============================================

enum Modes
{

    ONLY_CALC = 100,
    CALC_AND_SHOW = 200

};

//===============================================

enum Modes _parse_cmndln(int argc, char* argv[] FOR_LOGS(, LOG_PARAMS));

int _mandelbrot_exec(enum Modes mode FOR_LOGS(, LOG_PARAMS));

int _mandelbrot_eval(FOR_LOGS(LOG_PARAMS));

int _mandelbrot_show(FOR_LOGS(LOG_PARAMS));

//===============================================

#define parse_cmndln(argc, argv) \
       _parse_cmndln(argc, argv FOR_LOGS(, LOG_ARGS))

#define mandelbrot_exec(mode) \
       _mandelbrot_exec(mode FOR_LOGS(, LOG_ARGS))

#define mandelbrot_eval() \
       _mandelbrot_eval(FOR_LOGS(LOG_ARGS))

#define mandelbrot_show() \
       _mandelbrot_show(FOR_LOGS(LOG_ARGS))
    