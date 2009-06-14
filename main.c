#include <SDL.h>
#include "capture_v4l.h"

static struct video_mbuf vm; 	/* structure for frame buffer */
static struct video_mmap vmap;	/* structure for memory space for frame buffer */
int fd;
unsigned char *fbuffer;

void prep_capture()
{
	struct video_capability vcap;
	struct video_picture vp;
	struct video_channel vch[MAX_NO_CHANNEL];
	
	fd = CaptureV4LOpen( DEFAULT_DEVICE_NAME );
	CaptureV4LGetDeviceCapability(fd, &vcap);
	CaptureV4LDisplayDeviceCapability(vcap);
	CaptureV4LGetPictureInfo(fd, &vp);
	CaptureV4LDisplayPictureInfo(vp);
	
	CaptureV4LGetMemoryMapInfo( fd , &vm );
	CaptureV4LDisplayMemoryMapInfo(vm);
	
	if( CaptureV4LSelectChannel( fd , vch , 0 ) == -1 ) {
		fprintf( stderr , "Could not select channel.\n" );
		exit(-1);
    }
	
	CaptureV4LMemoryMapping( fd , vm );
	
	vmap.width = CAPTURE_IMAGE_WIDTH;
    vmap.height = CAPTURE_IMAGE_HEIGHT;
    vmap.format = VIDEO_PALETTE_YUV420P;
	
	if (CaptureV4LDoubleBufferingInitCapture(fd, &vmap) == -1)
		printf("WAARNNNNNNNNNING\n");
}


void pixel(SDL_Surface *s, int x, int y, int r, int g, int b)
{
	Uint32 *m;
	y *= s->w;
	m = (Uint32*) s->pixels + y + x;
	*m = SDL_MapRGB(s->format,r,g,b);
}


void yuv_to_rgb(int y, int u, int v, int *r, int *g, int *b)
{
	static float R,G,B;
	B = 1.164*(y - 16)                   + 2.018*(u - 128);
	G = 1.164*(y - 16) - 0.813*(v - 128) - 0.391*(u - 128);
	R = 1.164*(y - 16) + 1.596*(v - 128);
	*r = (int) R;
	*g = (int) G;
	*b = (int) B;
}


void show_image(SDL_Surface *s)
{
	int x;
	int y;
	unsigned char *buf;
	static int r,g,b;
	
	CaptureV4LDoubleBufferingCaptureWait(fd, &vmap);
	buf = CaptureV4LGetImage(vmap,vm);
// 	buf += 640*480;
	
	for(y=0;y<480;y++)
		for(x=0;x<640;x++)
		{
			yuv_to_rgb( *buf, 20, 60, &r, &g, &b);
			pixel(s, x,y, r,g,b);
			buf += 1;
		}
	
	CaptureV4LDoubleBufferingCaptureNextFrame( fd , &vmap );
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
		SDL_UpdateRect(s,0,0,0,0);
		SDL_FillRect(s,0,black_color);
		show_image(s);
		
		SDL_Delay(200);

	}

	exit(0);
}
