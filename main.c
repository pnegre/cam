#include <SDL.h>
#include "capture_v4l.h"

void prep_capture()
{
	int fd;
	struct video_capability vcap;
	struct video_picture vp;
	
	static struct video_mbuf vm; 	/* structure for frame buffer */
	static struct video_mmap vmap;	/* structure for memory space for frame buffer */
	
	fd = CaptureV4LOpen( DEFAULT_DEVICE_NAME );
	CaptureV4LGetDeviceCapability(fd, &vcap);
	CaptureV4LDisplayDeviceCapability(vcap);
	CaptureV4LGetPictureInfo(fd, &vp);
	CaptureV4LDisplayPictureInfo(vp);
	
	CaptureV4LGetMemoryMapInfo( fd , &vm );
	CaptureV4LDisplayMemoryMapInfo(vm);
	
	CaptureV4LMemoryMapping( fd , vm );
	
	vmap.width = CAPTURE_IMAGE_WIDTH;
    vmap.height = CAPTURE_IMAGE_HEIGHT;
    vmap.format = VIDEO_PALETTE_RGB24;
	
	CaptureV4LSimpleCapture(fd, &vmap);
}



int main ( void )
{
	SDL_Surface *s;
    Uint8 *k;
    SDL_Event e;
	Uint32 black_color;
	
	SDL_Init(SDL_INIT_VIDEO);
	s = SDL_SetVideoMode(640,480,0,SDL_HWSURFACE);
	black_color = SDL_MapRGB(s->format,0,0,0);
	
	prep_capture();
	
	while(1)
	{
	 	SDL_PollEvent(&e);
		k = SDL_GetKeyState(NULL);
		if (k[SDLK_ESCAPE]) break;
		SDL_Delay(25);

		SDL_UpdateRect(s,0,0,0,0);
		SDL_FillRect(s,0,black_color);
	}

	exit(0);
}
