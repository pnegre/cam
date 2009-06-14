#include <SDL.h>

#include "videodevice.h"

#define SC_W 640
#define SC_H 480





int main ( void )
{
	SDL_Surface *s, *fr;
    Uint8 *k;
    SDL_Event e;
	Uint32 black_color;
	
	VideoDevice vdev(SC_W,SC_H);
	
	SDL_Init(SDL_INIT_VIDEO);
	s = SDL_SetVideoMode(SC_W,SC_H,0,SDL_HWSURFACE);
	black_color = SDL_MapRGB(s->format,0,0,0);
	
	fr = SDL_CreateRGBSurface(SDL_HWSURFACE, SC_W,SC_H, 24,0,0,0,0);
	
	while(1)
	{
	 	SDL_PollEvent(&e);
		k = SDL_GetKeyState(NULL);
		if (k[SDLK_ESCAPE]) break;
		SDL_UpdateRect(s,0,0,0,0);
		SDL_FillRect(s,0,black_color);
		
		vdev.capture();
		vdev.YUVtoBGR((unsigned char*)fr->pixels);
		SDL_BlitSurface(fr,NULL,s,NULL);
		vdev.prepareCapture();
		
		SDL_Delay(50);

	}

	exit(0);
}
