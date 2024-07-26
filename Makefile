BIN = ./kyuubey
BUILD_DIR = out

CFLAGS = -I. -Iout $(shell $(SDL2_CONFIG) --cflags) -g -Wall -Wno-unneeded-internal-declaration -Wno-unused-function
LDFLAGS = $(shell $(SDL2_CONFIG) --libs)

SDL2_CONFIG = sdl2-config
SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o) $(BUILD_DIR)/lang.tab.o $(BUILD_DIR)/lang.yy.o
DEPS = $(OBJS:$(BUILD_DIR)/%.o=$(BUILD_DIR)/%.d)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

-include $(DEPS)

$(BUILD_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -MMD $< -o $@

$(BUILD_DIR)/lang.tab.c: lang.y
	bison -v --report=state -d $<
	mv lang.tab.? $(BUILD_DIR)/

$(BUILD_DIR)/lang.yy.c: lang.l
	flex -t $< > $@

parser-test: $(BIN)
	valgrind --suppressions=valgrind.suppressions --dsymutil=yes --leak-check=full --gen-suppressions=all $(BIN) parser-test

clean:
	-rm $(OBJS) $(DEPS) $(BIN)
