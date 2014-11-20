EXECUTABLE=bricksdl
SOURCES=box.c motion.c viewsdl.c main.c
OBJECTS=box.o motion.o viewsdl.o main.o

#CC=gcc
#CC=clang
SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)
CFLAGS+=-std=c99 -O3 $(SDL_CFLAGS)
LDFLAGS+=$(SDL_LDFLAGS) -lSDL_gfx

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -Wall -o $@

box.o: box.c box.h
	$(CC) $(CFLAGS) -Wall -c box.c

motion.o: motion.c box.h motion.h
	$(CC) $(CFLAGS) -Wall -c motion.c

viewsdl.o: viewsdl.c box.h motion.h viewsdl.h
	$(CC) $(CFLAGS) -Wall -c viewsdl.c

main.o: main.c box.h motion.h viewsdl.h
	$(CC) $(CFLAGS) -Wall -c main.c

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
