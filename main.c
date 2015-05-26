#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

typedef struct {
    /* 8x14 */
    unsigned char bitmap[14];
} egachar;

typedef struct {
    egachar charset[256];
} egafont;

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

    egafont font;
    FILE *f = fopen("cp437.ega", "r");
    fread(&font, sizeof(egafont), 1, f);
    fclose(f);

    SDL_RenderClear(renderer);

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            int bx = x * 8,
                by = y * 14;

            egachar c = font.charset[y * 16 + x];
            for (int row = 0; row < 14; ++row) {
                unsigned char rowdata = c.bitmap[row],
                              mask = 0x80;
                for (int offset = 0; offset < 8; ++offset) {
                    if (rowdata & mask) {
                        SDL_RenderDrawPoint(renderer, bx + offset, by + row);
                    }
                    mask /= 2;
                }
            }
        }
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(2000);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

/* vim: set sw=4 et: */
