CC=gcc

# Source and object files
SOURCES=$(wildcard *.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

LIBS=-lm

all: tennis

tennis: $(OBJECTS)
	$(CC) $(OBJECTS) -o tennis $(LIBS)

%.o: %.c
	$(CC) -c $< -o $@

.PHONY: clean
clean:
	rm -rf *.o
	rm -rf tennis
