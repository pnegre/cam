#
# macros
#
# GLUT_LDFLAG	= -L/usr/X11R6/lib -lglut -lGL -lGLU
LDFLAGS 	= $(GLUT_LDFLAG) -lm  -L/usr/lib -lSDL

GLUT_INCFLAG	= -I/usr/X11R6/include -I/usr/include/SDL
INCFLAGS	= $(GLUT_INCFLAG) 

CC		= gcc	
CFLAGS		= -O  -D_GNU_SOURCE=1 -D_REENTRANT

SRCS		= main.c capture_v4l.c
HDRS		= show_video_v4l.h
OBJS		= $(SRCS:.c=.o)
TARGET		= main

#
# compile
#
.c.o:
	$(CC) $(INCFLAGS) $(CFLAGS)  -c $<
#
# link 
#
$(TARGET) : $(OBJS) 
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

#
# compare time 
#
# capture_v4l.o : capture_v4l.c $(HDRS)


clean:
	rm -f $(OBJS) $(TARGET) *~
