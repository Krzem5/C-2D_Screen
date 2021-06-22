#include <screen.h>
#ifdef _MSC_VER
#define WINDOWS_LEAN_AND_MEAN 1
#include <windows.h>
#else
#error Non-Windows System not Supported!
#endif
#include <stdint.h>
#include <stdio.h>



void _callback(void){
	return;
}



LRESULT CALLBACK _wnd_proc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
	if (msg==WM_DESTROY){
		screen_deinit((screen_t*)GetWindowLongPtrW(hwnd,0));
		return 0;
	}
	if (msg==WM_SIZE){
		screen_t* s=(screen_t*)GetWindowLongPtrW(hwnd,0);
		s->ww=LOWORD(lp);
		s->wh=HIWORD(lp);
		return 0;
	}
	return DefWindowProc(hwnd,msg,wp,lp);
}



color_t _hsv_to_rgb(uint8_t h,uint8_t s,uint8_t v,uint8_t a){
	if (s==0){
		return CREATE_COLOR(v,v,v,a);
	}
	uint8_t i=h/43;
	uint8_t r=(h-(i*43))*6;
	uint8_t p=((uint32_t)v*(255-s))>>8;
	uint8_t q=((uint32_t)v*(255-(((uint32_t)s*r)>>8)))>>8;
	uint8_t t=((uint32_t)v*(255-(((uint32_t)s*(255-r))>>8)))>>8;
	if (!i){
		return CREATE_COLOR(v,t,p,a);
	}
	if (i==1){
		return CREATE_COLOR(q,v,p,a);
	}
	if (i==2){
		return CREATE_COLOR(p,v,t,a);
	}
	if (i==3){
		return CREATE_COLOR(p,q,v,a);
	}
	if (i==4){
		return CREATE_COLOR(t,p,v,a);
	}
	return CREATE_COLOR(v,p,q,a);
}



void screen_init(screen_t* s,const char* nm,int x,int y,int w,int h){
	uint32_t sz=0;
	while (*(nm+sz)){
		sz++;
	}
	WCHAR tmp[256]={0};
	MultiByteToWideChar(CP_UTF8,MB_PRECOMPOSED,nm,sz,tmp,256);
	HINSTANCE ci=GetModuleHandleW(NULL);
	WNDCLASSW wc={
		0,
		_wnd_proc,
		0,
		sizeof(screen_t*),
		ci,
		NULL,
		NULL,
		NULL,
		NULL,
		tmp
	};
	RegisterClassW(&wc);
	SetProcessDPIAware();
	s->cb=_callback;
	s->w=w;
	s->h=h;
	s->ww=w;
	s->wh=h;
	s->mem=VirtualAlloc(NULL,w*h*sizeof(color_t),MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
	s->_h=CreateWindowExW(0,tmp,tmp,0,x,y,w,h,NULL,NULL,ci,NULL);
	SetWindowLongPtrW(s->_h,GWL_EXSTYLE,0);
	SetWindowLongPtrW(s->_h,GWL_STYLE,WS_VISIBLE);
	SetWindowLongPtrW(s->_h,0,(LONG_PTR)s);
}



void screen_set_style(screen_t* s,uint8_t st){
	if (st==SCREEN_STYLE_NO_BORDER){
		SetWindowLongPtrW(s->_h,GWL_EXSTYLE,0);
		SetWindowLongPtrW(s->_h,GWL_STYLE,WS_VISIBLE);
	}
	else{
		SetWindowLongPtrW(s->_h,GWL_EXSTYLE,WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
		SetWindowLongPtrW(s->_h,GWL_STYLE,WS_OVERLAPPEDWINDOW|WS_VISIBLE);
	}
}



void screen_set_buffer(screen_t* s,uint32_t w,uint32_t h){
	if (w!=s->w||h!=s->h){
		s->w=w;
		s->h=h;
		VirtualFree(s->mem,0,MEM_RELEASE);
		s->mem=VirtualAlloc(NULL,s->w*s->h*sizeof(color_t),MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
	}
}



void screen_start(screen_t* s){
	HDC hdc=GetDC(s->_h);
	BITMAPINFO bmi={
		{
			sizeof(BITMAPINFOHEADER),
			s->w,
			s->h,
			1,
			sizeof(color_t)<<3,
			BI_RGB,
			0
		}
	};
	while (s->_h){
		bmi.bmiHeader.biWidth=s->w;
		bmi.bmiHeader.biHeight=s->h;
		StretchDIBits(hdc,0,0,s->ww,s->wh,0,0,s->w,s->h,(VOID*)s->mem,&bmi,DIB_RGB_COLORS,SRCCOPY);
		MSG msg;
		if (PeekMessage(&msg,s->_h,0,0,PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		s->cb();
	}
}



void screen_deinit(screen_t* s){
	if (s->_h){
		DestroyWindow(s->_h);
		s->_h=NULL;
	}
	VirtualFree(s->mem,0,MEM_RELEASE);
}
