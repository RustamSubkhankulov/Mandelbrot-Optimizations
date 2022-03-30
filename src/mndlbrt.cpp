#include "global_conf.h"
#include "mandelbrot/mandelbrot.h"

//===============================================

int main(int argc, char* argv[])
{

    #ifdef LOGS 

        FILE* logs_file = open_log_file(argv[1]);
        if (!logs_file)
            return -1;

    #endif 

    enum Modes mode = parse_cmndln(argc, argv);

    mandelbrot_exec(mode);

    #ifdef LOGS 
        close_log_file();
    #endif 
}