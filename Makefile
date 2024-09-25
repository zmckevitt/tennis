CC=gcc

#ifdef __APPLE__
CC=/usr/local/bin/gcc-14
#endif

# Source and object files
SOURCES=$(wildcard *.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

LIBS=-lm -lpthread

all: tennis

tennis: $(OBJECTS)
	$(CC) $(OBJECTS) -o tennis $(LIBS)

%.o: %.c
	$(CC) -c $< -o $@ $(LIBS)

.PHONY: clean
clean:
	rm -rf *.o
	rm -rf tennis
