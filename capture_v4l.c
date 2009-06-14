/*==========================================================================
  $Id$
  capture_v4l.c: Functions for capturing image using Video4Linux.
  Written by Naoyuki Ichimura, AIST, 2001.
==========================================================================*/

#include "capture_v4l.h"

/*=== Global variable for this module ===*/
static unsigned char *frame_buf;

/*============================================================================
  Functions for Device Open and Check
============================================================================*/
int CaptureV4LOpen( char *device_name )
{

    int fd;

/*======= Open Video4Linux device ==========================*/
    if( ( fd = open( device_name , O_RDWR ) ) == -1 ) {
	return -1;
    }

    return fd;

}

int CaptureV4LGetDeviceCapability( int fd , struct video_capability *vcap )
{

/*======= Get device capabilities ==========================*/
    if( ioctl( fd , VIDIOCGCAP , vcap ) == -1 ) {
	return -1;
    }

    return 0;

}

void CaptureV4LDisplayDeviceCapability( struct video_capability vcap )
{

    fprintf( stdout , "*** Device information ***\n" );

/*======= Display deivce name =============================*/
    fprintf( stdout , "Device name:%s.\n" , vcap.name );

/*======= Display deivce types ============================*/
    fprintf( stdout , "Type of device:\n" );
    if( vcap.type & VID_TYPE_CAPTURE ) {
	fprintf( stdout , "Capture is OK.\n" );
    } else {
	fprintf( stderr , "This device may NOT be used for capture. Are you sure about it ?\n" );
    }
    if( vcap.type & VID_TYPE_TUNER ) {
	fprintf( stdout , "Tuner exists.\n" );
    }
    if( vcap.type & VID_TYPE_TELETEXT ) {
	fprintf( stdout , "Teletext can be used.\n" );
    }
    if( vcap.type & VID_TYPE_OVERLAY ) {
	fprintf( stdout , "Overlay is OK.\n" );
    }
    if( vcap.type & VID_TYPE_CHROMAKEY ) {
	fprintf( stdout , "Overlay by chromakey is OK.\n" );
    }
    if( vcap.type & VID_TYPE_CLIPPING ) {
	fprintf( stdout , "Clipping overlay is OK.\n" );
    }
    if( vcap.type & VID_TYPE_FRAMERAM ) {
	fprintf( stdout , "Overlay for frame buffer is OK.\n" );
    }
    if( vcap.type & VID_TYPE_SCALES ) {
	fprintf( stdout , "Scaling by hardware can be used.\n" );
    }
    if( vcap.type & VID_TYPE_MONOCHROME ) {
	fprintf( stdout , "Capture of monochrome image is OK.\n" );
    }
    if( vcap.type & VID_TYPE_SUBCAPTURE ) {
	fprintf( stdout , "Capture of a part of image is OK.\n" );
    } 
    fprintf( stdout , "\n" );

/*======= Display number of video channels and audio ones ===*/ 
    fprintf( stdout , "Number of channels: %d\n" , vcap.channels );
    fprintf( stdout , "Number of audio devices: %d\n" , vcap.audios );

    if( vcap.channels > MAX_NO_CHANNEL ) {
	fprintf( stderr , "It seems that too many channels exist in this device: %d.\n" , vcap.channels );
    }

/*======= Display image size ================================*/
    fprintf( stdout , "Maximum width of image: %d\n" , vcap.maxwidth );
    fprintf( stdout , "Maximum height of image: %d\n", vcap.maxheight );
    fprintf( stdout , "Minimum width of image: %d\n" , vcap.minwidth );
    fprintf( stdout , "Minimum height of image: %d\n" , vcap.minheight );
    fprintf( stdout , "\n" );

}

int CaptureV4LGetChannelInfo( int fd , struct video_channel vch[MAX_NO_CHANNEL] , int no_channel )
{

    int i;

/*======= Get channel information ==========================*/
    for( i = 0 ; i < no_channel ; i++ ) {
	vch[i].channel = i;	/* channel number */
	if( ioctl( fd , VIDIOCGCHAN , &vch[i] ) == -1 ) {
	    return -1;
	}
    }

    return 0;

}

void CaptureV4LDisplayChannelInfo( struct video_channel vch[MAX_NO_CHANNEL] , int no_channel )
{

    int i;

    for( i = 0 ; i < no_channel ; i++ ) {
	fprintf( stdout , "*** Channel number: %d ***\n" , vch[i].channel );
	fprintf( stdout , "Channel name: %s\n" , vch[i].name );
	if( vch[i].flags & VIDEO_VC_TUNER ) {
	    fprintf( stdout , "This channel has a tuner.\n" );
	}
	if( vch[i].flags & VIDEO_VC_AUDIO ) {
	    fprintf( stdout , "This channel has a audio.\n" );
	}
	if( vch[i].type & VIDEO_TYPE_TV ) {
	    fprintf( stdout , "Channel type is TV.\n" );
	}
	if( vch[i].type & VIDEO_TYPE_CAMERA ) {
	    fprintf( stdout , "Channel type is camera.\n" );
	}
	if( vch[i].norm & VIDEO_MODE_NTSC ) {
	    fprintf( stdout , "Video mode is NTSC.\n" );
	}
	if( vch[i].norm & VIDEO_MODE_PAL ) {
	    fprintf( stdout , "Video mode is PAL.\n" );
	}
	if( vch[i].norm & VIDEO_MODE_SECAM ) {
	    fprintf( stdout , "Video mode is SECAM.\n" );
	}
    }
    fprintf( stdout , "\n" );

}

int CaptureV4LGetPictureInfo( int fd , struct video_picture *vp )
{

/*======= Get channel information ==========================*/
    if( ioctl( fd , VIDIOCGPICT , vp ) == -1 ) {
	return -1;
    }

    return 0;

}

void CaptureV4LDisplayPictureInfo( struct video_picture vp )
{

    fprintf( stdout , "*** Picture information ***\n" );
    fprintf( stdout , "Brightness: %d\n" , vp.brightness );
    fprintf( stdout , "Hue: %d\n" , vp.hue );
    fprintf( stdout , "Color: %d\n" , vp.colour );
    fprintf( stdout , "Contrast: %d\n" , vp.contrast );
    fprintf( stdout , "Whiteness: %d\n" , vp.whiteness );
    fprintf( stdout , "Depth: %d\n" , vp.depth );
    switch( vp.palette ) {
        case VIDEO_PALETTE_GREY:
	  fprintf( stdout , "Gray scale.\n" );
	  break;
	    
        case VIDEO_PALETTE_RGB565:
	  fprintf( stdout , "16bit RGB.\n" );
	  break;

        case VIDEO_PALETTE_RGB24:
	  fprintf( stdout , "24bit RGB.\n" );
	  break;

        case VIDEO_PALETTE_RGB32:
	  fprintf( stdout , "32bit RGB.\n" );
	  break;
	    
        case VIDEO_PALETTE_YUV422:
	  fprintf( stdout , "YUV422.\n" );
	  break;

        default:
	  break;  

    }

}

int CaptureV4LGetMemoryMapInfo( int fd , struct video_mbuf *vm )
{

/*======= Get channel information ==========================*/
    if( ioctl( fd , VIDIOCGMBUF , vm ) == -1 ) {
	return -1;
    }

    return 0;

}

void CaptureV4LDisplayMemoryMapInfo( struct video_mbuf vm )
{

    int i;

    fprintf( stdout , "Memory size: %d\n" , vm.size );
    fprintf( stdout , "Number of frames: %d\n" , vm.frames );
    for( i = 0 ; i < vm.frames ; i++ ) {
	fprintf( stdout , "Offset (frame %d): %d\n" , i , vm.offsets[i] );
    }

}

/*============================================================================
  Functions for preparation of capture
============================================================================*/
int CaptureV4LSelectChannel( int fd , struct video_channel vch[MAX_NO_CHANNEL] , int channel_no )
{

    vch[channel_no].norm = VIDEO_MODE_NTSC;
    if( ioctl( fd , VIDIOCSCHAN , &vch[channel_no] ) == -1 ) {
	return -1;
    }
    
    return 0;

}

int CaptureV4LMemoryMapping( int fd , struct video_mbuf vm )
{

    if( ( frame_buf = (unsigned char *)mmap( 0 , (size_t)vm.size , PROT_READ | PROT_WRITE , MAP_SHARED , fd , 0 ) ) == MAP_FAILED ) {
	return -1;
    }

    return 0;

}

/*============================================================================
  Functions for capture
============================================================================*/
int CaptureV4LSimpleCapture( int fd , struct video_mmap *vmap )
{

/*======= Begin capture ====================================*/
    if( ioctl( fd , VIDIOCMCAPTURE , vmap ) == -1 ) {
	return -1;
    }

/*======= Wait capture =====================================*/
    if( ioctl( fd , VIDIOCSYNC , &(vmap->frame) ) == -1 ) {
	return -1;
    }

}

int CaptureV4LDoubleBufferingInitCapture( int fd , struct video_mmap *vmap )
{

/*======= Capture image into frame buffers at beginning ====*/
    vmap->frame = 0;
    if( ioctl( fd , VIDIOCMCAPTURE , vmap ) == -1 ) {
	return -1;
    }
    vmap->frame = 1;
    if( ioctl( fd , VIDIOCMCAPTURE , vmap ) == -1 ) {
	return -1;
    }
    /*=== Set initial frame number ===*/
    vmap->frame = 0;

}

int CaptureV4LDoubleBufferingCaptureWait( int fd , struct video_mmap *vmap ) 
{

/*======= Wait Capture ====================================*/ 
    if( ioctl( fd , VIDIOCSYNC , &(vmap->frame) ) == -1 ) {
	return -1;
    }

    return 0;

}

int CaptureV4LDoubleBufferingCaptureNextFrame( int fd , struct video_mmap *vmap )
{

/*======= Capture next frame into current buffer ==========*/
    if( ioctl( fd , VIDIOCMCAPTURE , vmap ) == -1 ) {
	return -1;
    }
    
/*======= Change buffer ===================================*/
    if( vmap->frame == 0 ) {
	vmap->frame = 1;
    } else {
	vmap->frame = 0;
    }
    
    return 0;

}

/*============================================================================
  Functions for image array
============================================================================*/
unsigned char *CaptureV4LSetImage( struct video_mmap vmap , struct video_mbuf vm )
{

    unsigned char tmpc;
    unsigned char *pixel_pos;

    int i;
    
/*======= Change order of data from BGR to RGB ===============*/
    pixel_pos = frame_buf+vm.offsets[vmap.frame];
    for( i = 0 ; i < vmap.height*vmap.width ; i++ ) {
	tmpc = pixel_pos[2];
	pixel_pos[2] = pixel_pos[0]; 
	pixel_pos[0] = tmpc;
	pixel_pos += RGB;
    }

    return frame_buf+vm.offsets[vmap.frame];

}

void CaptureV4LSetImageDownSamplingForOpenGL( struct video_mmap vmap , struct video_mbuf vm , int down_sampling_rate , unsigned char *image , unsigned char *disp_image )
{

    unsigned char *pixel_pos;

    int i,j;
    int dheight,dwidth;
    
/*======= Change order of data from BGR(capture board order) to RGB for processing ===============*/
    dheight = vmap.height/down_sampling_rate;
    dwidth = vmap.width/down_sampling_rate;
    pixel_pos = frame_buf+vm.offsets[vmap.frame];
    for( i = 0 ; i < dheight ; i++ ) {
	for( j = 0 ; j < dwidth; j++ ) {
	    /* image for processing: RGB order */
	    image[((dheight-1-i)*dwidth+j)*RGB] = pixel_pos[2];
	    image[((dheight-1-i)*dwidth+j)*RGB+1] = pixel_pos[1]; 
	    image[((dheight-1-i)*dwidth+j)*RGB+2] = pixel_pos[0]; 
	    /* image for display: BRG order */
	    disp_image[((dheight-1-i)*dwidth+j)*RGB] = pixel_pos[0];
	    disp_image[((dheight-1-i)*dwidth+j)*RGB+1] = pixel_pos[1]; 
	    disp_image[((dheight-1-i)*dwidth+j)*RGB+2] = pixel_pos[2]; 
	    pixel_pos += RGB*down_sampling_rate;
	}
	pixel_pos += vmap.width*RGB*(down_sampling_rate-1);
    }

}
