CFLAGS = -I ./include
LIB    = ./libggfonts.a
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm
SOURCES = project.cpp ppm.cpp perryH.cpp ericS.cpp

all: project

project: $(SOURCES)
	g++ $(SOURCES) $(LIB) -Wall -Wextra $(LFLAGS) -o project

clean:
	rm -f project
	rm -f *.o
