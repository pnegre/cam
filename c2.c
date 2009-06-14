#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

// #include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

struct buffer {
	void *  start;
	size_t  length;
};

struct buffer * buffers = NULL;

int fd;
char *dev = "/dev/video0";

static int xioctl (int fd, int request, void *arg)
{
	int r;
	do r = ioctl (fd, request, arg);
	while (-1 == r && EINTR == errno);
	return r;
}

void die( char *m )
{
	printf("%s\n", m);
	exit(1);
}


void init1()
{
	struct video_capability cap;
	struct video_picture pic;
// 	struct video_format fmt;
	int bufsize;
	int min;

	if ((fd = open (dev, O_RDONLY, 0)) < 0)
		die("CAN'T OPEN");

	xioctl(fd, VIDIOCGCAP, &cap);

// 	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
// 		die("NO CAP_VIDEO_CAPTURE");
// 	
// 	if (!(cap.capabilities & V4L2_CAP_READWRITE))
// 		die("NO CAP_READWRITE");
// 	
// 	if (!(cap.capabilities & V4L2_CAP_STREAMING))
// 		die("NO CAP_STREAMING");
	
	printf("mwidth %d\n",cap.maxwidth);
	die("ei");
	
/*
	CLEAR (fmt);

	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = 640;
	fmt.fmt.pix.height      = 480;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YVU420;
	fmt.fmt.pix.field       = V4L2_FIELD_ANY;

	if (-1 == xioctl (fd, VIDIOC_S_FMT, &fmt))
		die("NO FMT");

	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage;

	bufsize = fmt.fmt.pix.sizeimage;
	printf("Bufsize = %d\n", bufsize);

	buffers = calloc (1, sizeof (*buffers));
	buffers[0].length = bufsize;
	buffers[0].start = malloc (bufsize);*/
}

// int read_frame()
// {
// 	if (-1 == read (fd, buffers[0].start, buffers[0].length))
// 	{
// 		switch (errno)
// 		{
// 			case EAGAIN:
// 				return 0;
// 				break;
// 			default:
// 				die("READ ERROR\n");
// 				break;
// 		}
// 	}
// 	return 1;
// 
// }
// 
// void shutdown_cam()
// {
// 	free(buffers[0].start);
// 	free(buffers);
// 	close(fd);
// }


int main()
{
	int i;
	init1();

// 	for (i=0; i<100; i++)
// 	{
// 		if (read_frame() == 1) { printf("."); }
// 	}
// 	printf("\n");
// 	shutdown_cam();
}
