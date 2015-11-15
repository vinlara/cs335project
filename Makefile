CFLAGS = -I ./include
LIB    = ./lib/fmod/libfmodex64.so ./libggfonts.a
LFLAGS =  $(LIB) -lrt -lX11 -lGLU -lGL -pthread -lm#-lXrandr

all: project

project: project.cpp ppm.cpp fmod.c
	g++ $(CFLAGS) project.cpp ppm.cpp fmod.c -Wall -Wextra $(LFLAGS) -o project

clean:
	rm -f project
	rm -f *.o
