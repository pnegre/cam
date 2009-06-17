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



class Processor
{
protected:
	IplImage *im, *red, *green, *blue;
	int w,h;
	VideoDevice *vdev;

public:
	Processor()
	{
		im = cvCreateImage( cvSize(SC_W,SC_H), IPL_DEPTH_8U, 3 );
		vdev = new VideoDevice(SC_W,SC_H);
		red = cvCreateImage( cvSize(SC_W,SC_H), IPL_DEPTH_8U, 1 );
		green = cvCreateImage( cvSize(SC_W,SC_H), IPL_DEPTH_8U, 1 );
		blue = cvCreateImage( cvSize(SC_W,SC_H), IPL_DEPTH_8U, 1 );
	}
	
	SDL_Surface *doit()
	{
		vdev->capture();
		vdev->YUVtoBGR((unsigned char*)im->imageData);
		
		
// 		cvSplit(im,blue,green,red,0);
// 		cvFloodFill(red,cvPoint(290,100),cvScalar(250),cvScalar(4),cvScalar(2),NULL,4,NULL);
// 		cvAdaptiveThreshold(green,red,200,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,3,5);
// 		cvErode(red,red,NULL,2);
// 		cvCanny(blue,red,9/*0,80,3);
// 		cvMerge(red,red,red,0,im);
// 		cvRectangle(im,cvPoint(285,95),cvPoint(295,105),cvScalar(255*/),1,8,0);
		
		vdev->prepareCapture();
		return cvToSdl(im);
	}
};






int main ( void )
{
	SDL_Surface *s;
    Uint8 *k;
    SDL_Event e;
	Uint32 black_color;
	
	SDL_Init(SDL_INIT_VIDEO);
	s = SDL_SetVideoMode(SC_W,SC_H,0,SDL_HWSURFACE);
	black_color = SDL_MapRGB(s->format,0,0,0);
	
	SDL_Surface *fr;
	Processor proc;
	while(1)
	{
	 	SDL_PollEvent(&e);
		k = SDL_GetKeyState(NULL);
		if (k[SDLK_ESCAPE]) break;
		SDL_UpdateRect(s,0,0,0,0);
		SDL_FillRect(s,0,black_color);
		
		fr = proc.doit();
		
		SDL_BlitSurface(fr,NULL,s,NULL);
		SDL_FreeSurface(fr);
		
		SDL_Delay(50);

	}

	exit(0);
}
