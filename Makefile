CFLAGS = -I ./include
##LIB    = ./libggfonts.so
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: project

project: project.cpp ppm.cpp
	g++ $(CFLAGS) project.cpp ppm.cpp libggfonts.a -Wall -Wextra $(LFLAGS) -o project

clean:
	rm -f project
	rm -f *.o
