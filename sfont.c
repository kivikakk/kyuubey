#include "sfont.h"

typedef struct {
    /* 8x14 */
    unsigned char bitmap[14];
} egachar;

typedef struct {
    egachar charset[256];
} egafont;


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

/* vim: set sw=4 et: */
