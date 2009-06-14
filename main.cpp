#include <SDL.h>

#include "videodevice.h"


VideoDevice vdev;


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
	B = 1.164*((float)y - 16.0)                            + 2.018*((float)u - 128.0);
	G = 1.164*((float)y - 16.0) - 0.813*((float)v - 128.0) - 0.391*((float)u - 128.0);
	R = 1.164*((float)y - 16.0) + 1.596*((float)v - 128.0);
	if (R>255) R = 255;
	if (G>255) G = 255;
	if (B>255) B = 255;
	if (R<0)   R = 0;
	if (G<0)   G = 0;
	if (B<0)   B = 0;
	*r = (int) R;
	*g = (int) G;
	*b = (int) B;
}


void show_image(SDL_Surface *s)
{
	int x;
	int y;
	unsigned char *buf, *uu, *vv;
	static int r,g,b;
	
	buf = vdev.capture();
 	uu = buf + 640*480;
	vv = uu + 320*240;
	
	for(y=0;y<480;y++)
		for(x=0;x<640;x++)
		{
			yuv_to_rgb( *buf, uu[x/2 + (y/2)*320], vv[x/2 + (y/2)*320], &r, &g, &b);
			pixel(s, x,y, r,g,b);
			buf += 1;
		}
	
	vdev.prepareCapture();
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
