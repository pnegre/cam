#include "videodevice.h"


VideoDevice::VideoDevice(int width, int height)
{
	struct video_capability vcap;
	struct video_picture vp;
	struct video_channel vch[MAX_NO_CHANNEL];
	
	fd = CaptureV4LOpen( (char*) DEFAULT_DEVICE_NAME );
	CaptureV4LGetDeviceCapability(fd, &vcap);
	CaptureV4LDisplayDeviceCapability(vcap);
	CaptureV4LGetPictureInfo(fd, &vp);
	CaptureV4LDisplayPictureInfo(vp);
	
	CaptureV4LGetMemoryMapInfo( fd , &vm );
	CaptureV4LDisplayMemoryMapInfo(vm);
	
	CaptureV4LMemoryMapping( fd , vm );
	
	vmap.width = width;
	vmap.height = height;
	vmap.format = VIDEO_PALETTE_YUV420P;
	
	if (CaptureV4LDoubleBufferingInitCapture(fd, &vmap) == -1)
		printf("WAARNNNNNNNNNING\n");
}

void VideoDevice::capture()
{
	CaptureV4LDoubleBufferingCaptureWait(fd, &vmap);
}

// Torna els píxels en RAW (en el format que la càmera té configurat)
unsigned char *VideoDevice::raw()
{
	return CaptureV4LGetImage(vmap,vm);
}
	
void VideoDevice::prepareCapture()
{
	CaptureV4LDoubleBufferingCaptureNextFrame( fd , &vmap );
}

static void yuv_to_rgb(int y, int u, int v, int &r, int &g, int &b)
{
	b = (int) (1.164*((float)y - 16.0)                            + 2.018*((float)u - 128.0));
	g = (int) (1.164*((float)y - 16.0) - 0.813*((float)v - 128.0) - 0.391*((float)u - 128.0));
	r = (int) (1.164*((float)y - 16.0) + 1.596*((float)v - 128.0));
	if      (r>255) r = 255;
	else if (r<0)   r = 0;
	if      (g>255) g = 255;
	else if (g<0)   g = 0;
	if      (b>255) b = 255;
	else if (b<0)   b = 0;
	
}


// Funció que copia dins "dst" (ha de tenir la grandària apropiada) els píxels
// en format RGB (BGR en realitat) des de la font de la càmera (format YUV420P)
void VideoDevice::YUVtoBGR(unsigned char *dst)
{
	int x,y;
	unsigned char *buf, *uu, *vv;
	static int r,g,b;
	int w = vmap.width;
	int h = vmap.height;
	
	buf = CaptureV4LGetImage(vmap,vm);
	uu = buf + h*w;
	vv = uu + h*w/4;
	
	for(y=0;y<h;y++)
		for(x=0;x<w;x++)
		{
			yuv_to_rgb( *buf, uu[x/2 + (y/2)*320], vv[x/2 + (y/2)*320], r,g,b);
			*dst++ = b;
			*dst++ = g;
			*dst++ = r;
			buf += 1;
		}
}



