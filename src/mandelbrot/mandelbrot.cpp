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

int _mandelbrot_exec(enum Modes mode FOR_LOGS(, LOG_PARAMS))
{
    mndlbrt_log_report();

    if (mandelbrot_eval() == -1)
        return -1;

    switch(mode)
    {
        case ONLY_CALC:
        {
            return 0;
        }
        
        case CALC_AND_SHOW:
        {
            return mandelbrot_show();
        }

        default:
        {
            error_report(INV_MANDEL_MODE;
            return -1;
        }
    }

    return 0;
}

//-----------------------------------------------

int _mandelbrot_eval(FOR_LOGS(LOG_PARAMS))
{
    mndlbrt_log_report();

    return 0;
}

//-----------------------------------------------

int _mandelbrot_show(FOR_LOGS(LOG_PARAMS))
{
    mndlbrt_log_report();

    return 0;
}