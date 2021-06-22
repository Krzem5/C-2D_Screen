#ifndef __SCREEN_H__
#define __SCREEN_H__ 1
#ifdef _MSC_VER
#define WINDOWS_LEAN_AND_MEAN 1
#include <windows.h>
#else
#error Non-Windows System not Supported!
#endif
#include <stdint.h>



#define SCREEN_STYLE_NO_BORDER 0
#define SCREEN_STYLE_BORDER 1
#define CREATE_COLOR(r,g,b,a) ((((color_t)(a))<<24)|(((color_t)(r))<<16)|(((color_t)(g))<<8)|((color_t)(b)))
#define CREATE_COLOR_HSV(h,s,l,a) _hsv_to_rgb(h,s,l,a)



typedef uint32_t color_t;



typedef struct __SCREEEN{
	uint32_t w;
	uint32_t h;
	uint32_t ww;
	uint32_t wh;
	color_t* mem;
	void (*cb)(void);
	HWND _h;
} screen_t;



color_t _hsv_to_rgb(uint8_t h,uint8_t s,uint8_t v,uint8_t a);



void screen_init(screen_t* s,const char* nm,int x,int y,int w,int h);



void screen_set_style(screen_t* s,uint8_t st);



void screen_set_buffer(screen_t* s,uint32_t w,uint32_t h);



void screen_start(screen_t* s);



void screen_deinit(screen_t* s);



#endif
