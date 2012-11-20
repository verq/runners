CXX=gcc

CFLAGS= -std=c99 -O3 -Wall -I/usr/local/include -I/usr/X11R6/include -I/usr/include -I/opt/include -Wall

LFLAGS= -L/usr/X11R6/lib -L/opt/lib -lGL -lGLU -lglut -lX11 -lXext -lpthread -lm

all:
	${CXX} ${CFLAGS} ${LFLAGS} *.c
	
clean:
	rm a.out