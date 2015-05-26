#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

typedef struct {
    /* 8x14 */
    unsigned char bitmap[14];
} egachar;

typedef struct {
    egachar charset[256];
} egafont;

typedef struct {
    SDL_Texture *charset[256];
} sdlfont;

sdlfont *read_ega_sdlfont(SDL_Renderer *renderer, const char *filename) {
    egafont font;
    FILE *f = fopen(filename, "r");
    fread(&font, sizeof(egafont), 1, f);
    fclose(f);

    sdlfont *sfont = malloc(sizeof(*sfont));

    for (int i = 0; i < 256; ++i) {
        egachar c = font.charset[i];
        SDL_Texture *t = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, 8, 14);
        SDL_SetRenderTarget(renderer, t);
        SDL_RenderClear(renderer);

        for (int row = 0; row < 14; ++row) {
            unsigned char rowdata = c.bitmap[row],
                          mask = 0x80;
            for (int offset = 0; offset < 8; ++offset) {
                if (rowdata & mask) {
                    SDL_RenderDrawPoint(renderer, offset, row);
                }
                mask /= 2;
            }
        }

        sfont->charset[i] = t;
    }

    SDL_SetRenderTarget(renderer, NULL);

    return sfont;
}

void render_sfont(SDL_Renderer *renderer, sdlfont *sfont, int character, int x, int y) {
    SDL_Rect src = { 0, 0, 8, 14 };
    SDL_Rect dest = { x, y, 8, 14 };
    SDL_RenderCopy(renderer, sfont->charset[character], &src, &dest);
}

void free_sdlfont(sdlfont *sfont) {
    for (int i = 0; i < 256; ++i) {
        SDL_DestroyTexture(sfont->charset[i]);
    }
    free(sfont);
}

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

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

/* vim: set sw=4 et: */
