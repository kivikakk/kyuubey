#include "renderer.h"
#include "text.h"

static void loop(void);

int main(int argc, char **argv) {
    int r = renderer_init();
    if (r) {
        return r;
    }

    loop();

    renderer_quit();

    return 0;
}

#define FPS 60  /* Technically: vsync rate */
#define TICK_LENGTH (1000 / FPS)
#define FLIP_CURSOR 16

void loop(void) {
    SDL_Event event;
    Uint32 last_tick = SDL_GetTicks();

    text_refresh();
    int until_flip = FLIP_CURSOR;

    while (1) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    return;

                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    printf(
                        "%s %s %06d\n",
                        event.key.state == SDL_PRESSED ? "down" : "up  ",
                        SDL_GetKeyName(event.key.keysym.sym),
                        event.key.keysym.mod);
                    break;
            }
        }

        text_refresh();

        Uint32 next_tick = last_tick + TICK_LENGTH;
        last_tick = SDL_GetTicks();

        if (last_tick < next_tick) {
            SDL_Delay(next_tick - last_tick);
            last_tick = SDL_GetTicks();
        }

        --until_flip;
        if (until_flip == 0) {
            until_flip = FLIP_CURSOR;
            text_cursor_toggle();
        }
    }
}

/* vim: set sw=4 et: */
