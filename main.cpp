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
	IplImage *im, *res;
	int w,h;
	VideoDevice *vdev;
	CvMemStorage* storage;
	CvSeq *contour;

public:
	Processor()
	{
		im = cvCreateImage( cvSize(SC_W,SC_H), IPL_DEPTH_8U, 3 );
		res = cvCreateImage( cvSize(SC_W,SC_H), IPL_DEPTH_8U, 1 );
		vdev = new VideoDevice(SC_W,SC_H);
		storage = cvCreateMemStorage(0);
	}
	
	SDL_Surface *doit()
	{
		vdev->capture();
		vdev->YUVtoBGR((unsigned char*)im->imageData);
		
		int i,j;
		int r,g,b;
		unsigned char *buf = (unsigned char*)im->imageData;
		unsigned char *dstbuf = (unsigned char*) res->imageData;
		for(i=0;i<SC_W;i++)
			for(j=0;j<SC_H;j++)
			{
				b = *buf++; g = *buf++; r = *buf++;
				if ((r > b+90) && (r > g+90))
					*dstbuf++ = 255;
				else
					*dstbuf++ = 0;
			}

		cvErode(res,res,NULL,2);
		cvDilate(res,res,NULL,2); 
		
		cvClearMemStorage( storage );
		
		cvFindContours( res, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
		for( ; contour != 0; contour = contour->h_next )
		{
			CvPoint pt1,pt2;
			CvRect r = cvContourBoundingRect(contour,1);
			pt1.x=r.x;
			pt1.y=r.y;
			pt2.x=r.x+r.width;
			pt2.y=r.y+r.height;
			if ((r.width>20) && (r.height>10))
				cvRectangle(im,pt1,pt2,CV_RGB( 255, 0, 0 ),1);
		}
		
// 		cvMerge(res,res,res,0,im);
		
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

	}

	exit(0);
}
