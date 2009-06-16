#include <SDL.h>
#include <opencv/cv.h>
#include "highgui.h"

#include "videodevice.h"

#define SC_W 640
#define SC_H 480



SDL_Surface *cvToSdl(IplImage *opencvimg)
{
	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)opencvimg->imageData,
		opencvimg->width,
		opencvimg->height,
		opencvimg->depth*opencvimg->nChannels,
		opencvimg->widthStep,
		0xff0000, 0x00ff00, 0x0000ff, 0
		);
	return surface;
}



int main ( void )
{
	SDL_Surface *s;
    Uint8 *k;
    SDL_Event e;
	Uint32 black_color;
	
	VideoDevice vdev(SC_W,SC_H);
	
	SDL_Init(SDL_INIT_VIDEO);
	s = SDL_SetVideoMode(SC_W,SC_H,0,SDL_HWSURFACE);
	black_color = SDL_MapRGB(s->format,0,0,0);
	
// 	fr = SDL_CreateRGBSurface(SDL_HWSURFACE, SC_W,SC_H, 24,0,0,0,0);
	
	IplImage * im = cvCreateImage( cvSize(SC_W,SC_H), IPL_DEPTH_8U, 3 );
	IplImage * im2 = cvCreateImage( cvSize(SC_W,SC_H), IPL_DEPTH_8U, 3 );
	SDL_Surface *fr;
	while(1)
	{
	 	SDL_PollEvent(&e);
		k = SDL_GetKeyState(NULL);
		if (k[SDLK_ESCAPE]) break;
		SDL_UpdateRect(s,0,0,0,0);
		SDL_FillRect(s,0,black_color);
		
		vdev.capture();
		vdev.YUVtoBGR((unsigned char*)im->imageData);
		cvSmooth(im,im2, CV_GAUSSIAN, 7,7);
		fr = cvToSdl(im2);
		SDL_BlitSurface(fr,NULL,s,NULL);
		SDL_FreeSurface(fr);
		vdev.prepareCapture();
		
		SDL_Delay(50);

	}

	exit(0);
}
