#pragma once 

#include "../global_conf.h"

//===============================================

#ifdef LOGS

    #define MNDLBRT_LOGS

#endif 

//===============================================

static const int Init_x_size = 768;
static const int Init_y_size = 512;

static const double Init_x_shift = -64.f;
static const double Init_y_shift = 0.f;

#define X_SIZE 768
#define Y_SIZE 512

static const double Mul_x = 1/128.f;
static const double Mul_y = 1/128.f;
//static const float Mul = 1/128.f;

static const double Init_scale_factor = 0.6f;

//===============================================

static const double Scale_step = 0.1; 

static const double X_shift_step = 10.f;
static const double Y_shift_step = 10.f;

//===============================================

// static const int Init_x_pos = 100;
// static const int Init_y_pos = 100;

//===============================================

static const double Max_r2  = 4.f;

static const int Check_num = 512;

//===============================================

static const char Font_filename[] = "/home/rustam/Desktop/code/SSE/SSE-Optimizations/src/fonts/font.ttf";

static const int Fps_buf_size  = 1024;

static const int Fps_char_size = 32;

static const sf::Color Fps_color = sf::Color::Green;