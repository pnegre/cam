#include <deque>
#include <SDL.h>
#include <opencv/cv.h>
#include "highgui.h"

#include "videodevice.h"

#define SC_W 640
#define SC_H 480

char *CLASSIFIER_FILENAME = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml";



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
	CvHaarClassifierCascade* cascade;
	CvMemStorage* storage;
	
	std::deque<CvRect> rects;

public:
	Processor()
	{
		im = cvCreateImage( cvSize(SC_W,SC_H), IPL_DEPTH_8U, 3 );
		vdev = new VideoDevice(SC_W,SC_H);
		cascade = (CvHaarClassifierCascade*)cvLoad( CLASSIFIER_FILENAME, 0, 0, 0 );
		storage = cvCreateMemStorage(0);
	}
	
	
	void doCalculations()
	{
		CvRect r;
		int x=0, y=0, w=0 ,h=0;
		
		for(std::deque<CvRect>::iterator i=rects.begin(); i != rects.end(); i++)
		{
			r = *i;
			printf("%d %d %d %d\n", r.x, r.y, r.width, r.height);
		}
	}
	
	
	void inData(CvRect r)
	{
		int sz = rects.size();
		if (sz > 10)
		{
			// Calcular mitjanes i variances
			doCalculations();			
			rects.pop_front();
		}
		
		rects.push_back(r);
	}
	
	SDL_Surface *doit()
	{
		vdev->capture();
		vdev->YUVtoBGR((unsigned char*)im->imageData);
		
// 		IplImage *fr = cvCreateImage( cvSize(im->width,im->height),
// 			IPL_DEPTH_8U, im->nChannels );
// 		cvCopy(im,fr,0);
		
		cvClearMemStorage( storage );
		CvSeq* faces = cvHaarDetectObjects( im, cascade, storage,
			1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
			cvSize(150, 150) );
		
		CvPoint pt1, pt2;
		for( int i = 0; i < (faces ? faces->total : 0); i++ )
        {
			// Create a new rectangle for drawing the face
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
			
			// Find the dimensions of the face,and scale it if necessary
			pt1.x = r->x;
			pt2.x = (r->x+r->width);
			pt1.y = r->y;
			pt2.y = (r->y+r->height);

			// Draw the rectangle in the input image
			cvRectangle( im, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
			
			inData(*r);
			break;
		}
		
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
// 		SDL_FillRect(s,0,black_color);
		
		fr = proc.doit();
		
		SDL_BlitSurface(fr,NULL,s,NULL);
		SDL_FreeSurface(fr);
	}

	exit(0);
}
