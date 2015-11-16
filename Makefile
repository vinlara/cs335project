LIB    = ./libggfonts.a
FLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm -Wall -Wextra
SOURCES = project.cpp ppm.cpp perryH.cpp ericS.cpp

all: project

project: $(SOURCES)
	g++ $(SOURCES) $(LIB) $(FLAGS) -o project

clean:
	rm -f project
	rm -f *.o
