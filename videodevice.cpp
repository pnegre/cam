#include "videodevice.h"


VideoDevice::VideoDevice()
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

unsigned char *VideoDevice::capture()
{
	CaptureV4LDoubleBufferingCaptureWait(fd, &vmap);
	return CaptureV4LGetImage(vmap,vm);
}
	
void VideoDevice::prepareCapture()
{
	CaptureV4LDoubleBufferingCaptureNextFrame( fd , &vmap );
}




