EXECUTABLE=bricksdl
SOURCES=box.c motion.c viewsdl.c main.c
OBJECTS=box.o motion.o viewsdl.o main.o

#CC=gcc
#CC=clang
SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)
CFLAGS+=-std=c99 -O3 -Wall $(SDL_CFLAGS)
LDFLAGS+=$(SDL_LDFLAGS)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -Wall -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
