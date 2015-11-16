CFLAGS = -I ./include
LIB    = ./libggfonts.a
LFLAGS =  $(LIB) -lrt -lX11 -lGLU -lGL -pthread -lm#-lXrandr

all: project

project: project.cpp ppm.cpp perryH.cpp ericS.cpp
	g++ $(CFLAGS) project.cpp ppm.cpp perryH.cpp ericS.cpp -Wall -Wextra $(LFLAGS) -o project

clean:
	rm -f project
	rm -f *.o
