SDL2_CONFIG = /usr/local/Cellar/sdl2/2.0.3/bin/sdl2-config
CFLAGS := $(shell $(SDL2_CONFIG) --cflags)
LDFLAGS = $(shell $(SDL2_CONFIG) --libs)
SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=out/%.o)

all: vga

vga: $(OBJS)
	gcc $(LDFLAGS) $(OBJS) -o $@

out/%.o: %.c
	gcc -c $(CFLAGS) $< -o $@

clean:
	-rm $(OBJS) vga
