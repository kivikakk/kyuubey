BIN = kyuubey
BUILD_DIR = out

CFLAGS = $(shell $(SDL2_CONFIG) --cflags) -Wall
LDFLAGS = $(shell $(SDL2_CONFIG) --libs) -lSDL2main

SDL2_CONFIG = /usr/local/bin/sdl2-config
SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:$(BUILD_DIR)/%.o=$(BUILD_DIR)/%.d)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

-include $(DEPS)

$(BUILD_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -MMD $< -o $@

clean:
	-rm $(OBJS) $(DEPS) $(BIN)
