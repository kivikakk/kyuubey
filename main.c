#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#include "sfont.h"

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
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

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    sdlfont *sfont = read_ega_sdlfont(renderer, "cp437.ega");

    SDL_RenderClear(renderer);

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            render_sfont(renderer, sfont, y * 16 + x, x * 9, y * 15);
        }
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(2000);

    free_sdlfont(sfont);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

/* vim: set sw=4 et: */
