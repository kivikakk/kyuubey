#include "renderer.h"
#include "text.h"

SDL_Window *window = 0;
SDL_Renderer *renderer = 0;
sdlfont_t *sfont = 0;

int renderer_init(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow(
        "vga", 
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        80 * FONT_WIDTH,
        25 * FONT_HEIGHT,
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

    sfont = read_raw_sdlfont("cp437.vga");

    text_init();

    return 0;
}

void renderer_quit(void) {
    free_sdlfont(sfont);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
