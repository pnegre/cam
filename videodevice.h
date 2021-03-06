#include "capture_v4l.h"


class VideoDevice
{
protected:
	
	struct video_mbuf vm; 	/* structure for frame buffer */
	struct video_mmap vmap;	/* structure for memory space for frame buffer */
	unsigned char *fbuffer;
	int fd;

public:
	
	VideoDevice(int width, int height);
	void capture();
	unsigned char *raw();
	void getGray(unsigned char *dst);
	void prepareCapture();
	void YUVtoBGR(unsigned char *dst);
};

