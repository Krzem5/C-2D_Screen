#include <screen.h>
#include <stdio.h>



screen_t s;



void update(void){
	static uint8_t j=0;
	screen_set_buffer(&s,s.ww>>2,s.wh>>2);
	for (uint32_t i=0;i<s.w*s.h;i++){
		*(s.mem+i)=CREATE_COLOR_HSV(((i%s.w)-(i/s.w)-j)&0xff,255,255,255);
	}
	j++;
}



int main(int argc,const char** argv){
	screen_init(&s,"Test Screen",0,0,1920,1080);
	s.cb=update;
	screen_set_style(&s,SCREEN_STYLE_BORDER);
	screen_start(&s);
	return 0;
}
