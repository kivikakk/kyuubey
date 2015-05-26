SDL2_CONFIG = /usr/local/bin/sdl2-config
CFLAGS := $(shell $(SDL2_CONFIG) --cflags) -Wall
LDFLAGS = $(shell $(SDL2_CONFIG) --libs) -lSDL2main
SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=out/%.o)

all: vga

vga: $(OBJS)
	gcc $(LDFLAGS) $(OBJS) -o $@

out/%.o: %.c
	gcc -c $(CFLAGS) $< -o $@

clean:
	-rm $(OBJS) vga
