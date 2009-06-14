/*==========================================================================
  $Id$
  capture_v4l.h: Header file for capturing image using Video4Linux.
  Written by Naoyuki Ichimura, AIST, 2001.
==========================================================================*/

#include <stdio.h> 
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <sys/types.h> 		/* for open() */
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/ioctl.h> 		/* for ioctl() */

#include <unistd.h>		/* for mmap() */
#include <sys/mman.h>

#include <linux/videodev.h> 	/* for V4L */

/*=====================================================
  Definitions for Video4Linux
=====================================================*/
#define DEFAULT_DEVICE_NAME	"/dev/video0"	/* device file name */
#define MAX_NO_CHANNEL		10		/* maximum number of channel of frame grabber */
#define CAPTURE_IMAGE_WIDTH	640		/* image width */	
#define CAPTURE_IMAGE_HEIGHT	480		/* image height */

#define RGB			3 		/* number of bit plane */

#define COMPOSITE1		0		/* for IO-DATA GV-VCP2M/PCI */
#define COMPOSITE2		1
#define S_VIDEO			2

#define DOWN_SAMPLING_RATE	2		/* to remove even number field */

#define IMAGE_WIDTH_DS		CAPTURE_IMAGE_WIDTH/DOWN_SAMPLING_RATE 	/* image size after down sampling */
#define IMAGE_HEIGHT_DS		CAPTURE_IMAGE_HEIGHT/DOWN_SAMPLING_RATE

namespace C4L
{


/*=====================================================
  Function Prototypes
=====================================================*/
int CaptureV4LOpen( char *device_name );
int CaptureV4LGetDeviceCapability( int fd , struct video_capability *vcap );
void CaptureV4LDisplayDeviceCapability( struct video_capability vcap );
int CaptureV4LGetChannelInfo( int fd , struct video_channel vch[MAX_NO_CHANNEL] , int no_channel );
void CaptureV4LDisplayChannelInfo( struct video_channel vch[MAX_NO_CHANNEL] , int no_channel );
int CaptureV4LGetPictureInfo( int fd , struct video_picture *vp );
void CaptureV4LDisplayPictureInfo( struct video_picture vp );
int CaptureV4LGetMemoryMapInfo( int fd , struct video_mbuf *vm );
void CaptureV4LDisplayMemoryMapInfo( struct video_mbuf vm );

int CaptureV4LSelectChannel( int fd , struct video_channel vch[MAX_NO_CHANNEL] , int channel_no );
int CaptureV4LMemoryMapping( int fd , struct video_mbuf vm );

int CaptureV4LSimpleCapture( int fd , struct video_mmap *vmap );
int CaptureV4LDoubleBufferingInitCapture( int fd , struct video_mmap *vmap );
int CaptureV4LDoubleBufferingCaptureWait( int fd , struct video_mmap *vmap ) ;
int CaptureV4LDoubleBufferingCaptureNextFrame( int fd , struct video_mmap *vmap );

unsigned char *CaptureV4LSetImage( struct video_mmap vmap , struct video_mbuf vm );
unsigned char *CaptureV4LGetImage( struct video_mmap vmap , struct video_mbuf vm );
void CaptureV4LSetImageDownSamplingForOpenGL( struct video_mmap vmap , struct video_mbuf vm , int down_sampling_rate , unsigned char *image , unsigned char *disp_image );

};
