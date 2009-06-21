#include <deque>
#include <math.h>

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
	
	bool goodPhoto;
	CvRect photoRect;
	SDL_Surface *photo;
	
	Processor()
	{
		im = cvCreateImage( cvSize(SC_W,SC_H), IPL_DEPTH_8U, 3 );
		vdev = new VideoDevice(SC_W,SC_H);
		cascade = (CvHaarClassifierCascade*)cvLoad( CLASSIFIER_FILENAME, 0, 0, 0 );
		storage = cvCreateMemStorage(0);
		goodPhoto = false;
		photo = NULL;
	}
	
	
	void takePhoto(IplImage *image)
	{
		IplImage *subRect = cvCreateImage( cvSize(photoRect.width,photoRect.height), IPL_DEPTH_8U, 3 );
		cvSetImageROI(image,photoRect);
		cvCopy(image,subRect,NULL);
		cvResetImageROI(image);
		photo = cvToSdl(subRect);
		cvSaveImage("/tmp/image.jpg",subRect);
		cvReleaseImage(&subRect);
	}
	
	
	bool isDataAccurate()
	{
		CvRect r;
		float mx=0, my=0, mw=0, mh=0;
		float sz = rects.size();
		
		for(std::deque<CvRect>::iterator i=rects.begin(); i != rects.end(); i++)
		{
			r = *i;
			mx+=r.x; my+=r.y; mw+=r.width; mh+=r.height;
		}
		
		mx/=sz; my/=sz; mw/=sz; mh/=sz;
		
		float vx=0, vy=0, vw=0 ,vh=0;
		for(std::deque<CvRect>::iterator i=rects.begin(); i != rects.end(); i++)
		{
			r = *i;
			vx += pow(r.x      - mx, 2);
			vy += pow(r.y      - my, 2);
			vw += pow(r.width  - mw, 2);
			vh += pow(r.height - mh, 2);
		}
		
		vx/=sz; vy/=sz; vw/=sz; vh/=sz;
		vx = sqrt(vx);
		vy = sqrt(vy);
		vw = sqrt(vw);
		vh = sqrt(vh);
		
// 		printf("%f %f %f %f\n",vx,vy,vw,vh);
		if ((vx < 3.0) and (vy < 3.0) and (vw < 3.0) and (vh < 3.0))
		{
			photoRect.x = mx-mx*0.1;
			photoRect.y = my-my*0.1;
			photoRect.width  = mw + mw*0.2;
			photoRect.height = mh + mh*0.2;
			return true;
		}
		return false;
		
	}
	
	
	void inData(CvRect r)
	{
		int sz = rects.size();
		
		goodPhoto = false;
		
		if (sz >= 20)
		{
			if (isDataAccurate())
			{
				printf("OK, PHOTO!!\n");
				goodPhoto = true;
			}
			
			rects.pop_front();
		}
		
		rects.push_back(r);
	}
	
	SDL_Surface *doit()
	{
		vdev->capture();
		vdev->YUVtoBGR((unsigned char*)im->imageData);
		
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
			
			inData(*r);
			if (goodPhoto)
				takePhoto(im);
			
			// Draw the rectangle in the input image
			cvRectangle( im, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
				
			break;
		}
		
		vdev->prepareCapture();
		return cvToSdl(im);
	}
	
	SDL_Surface *getPhoto()
	{
		return photo;
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
	int state = 1;
	while(1)
	{
	 	SDL_PollEvent(&e);
		k = SDL_GetKeyState(NULL);
		if (k[SDLK_ESCAPE]) break;
		SDL_UpdateRect(s,0,0,0,0);
		
		if (state == 1)
		{
			fr = proc.doit();
			if (proc.goodPhoto)
				state = 2;
			
			SDL_BlitSurface(fr,NULL,s,NULL);
			SDL_FreeSurface(fr);
		}
		else if (state == 2)
		{
			SDL_FillRect(s,0,black_color);
			SDL_BlitSurface(proc.getPhoto(), NULL, s, NULL);
			if (k[SDLK_SPACE])
			{
				SDL_FreeSurface(proc.getPhoto());
				proc.goodPhoto = false;
				state = 1;
			}
		}
		
		
	}

	exit(0);
}
