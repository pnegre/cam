#include "capture_v4l.h"

int main()
{
	int fd;
	struct video_capability vcap;
	
	fd = CaptureV4LOpen( DEFAULT_DEVICE_NAME );
	CaptureV4LGetDeviceCapability(fd, &vcap);
	CaptureV4LDisplayDeviceCapability(vcap);
	
// 	ShowVideoInitCaptureDevice( DEFAULT_DEVICE_NAME , video_input );
	printf("EP");
}
