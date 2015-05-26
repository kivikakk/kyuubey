#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#include "sfont.h"
#include "text.h"

static SDL_Window *window = 0;
static SDL_Renderer *renderer = 0;
static sdlfont *sfont = 0;

static int init(void);
static void loop(void);

int main(int argc, char **argv) {
    int r = init();
    if (r) {
        return r;
    }

    loop();

    free_sdlfont(sfont);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

int init(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow(
        "vga", 
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        80 * 8,
        25 * 14,
        0);

    if (!window) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    sfont = read_ega_sdlfont(renderer, "cp437.ega");

    text_init();

    return 0;
}

void loop(void) {
    SDL_Event event;
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

        SDL_RenderClear(renderer);


        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }
}

/* vim: set sw=4 et: */
