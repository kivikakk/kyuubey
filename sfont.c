#include "sfont.h"
#include "renderer.h"

static int colors[16] = {
    0x000000,
    0x0000AA,
    0x00AA00,
    0x00AAAA,
    0xAA0000,
    0xAA00AA,
    0xAA5500,
    0xAAAAAA,
    0x555555,
    0x5555FF,
    0x55FF55,
    0x55FFFF,
    0xFF5555,
    0xFF55FF,
    0xFFFF55,
    0xFFFFFF,
};

typedef struct {
    /* 8x14 */
    unsigned char bitmap[14];
} egachar;

typedef struct {
    egachar charset[256];
} egafont;

sdlfont *read_ega_sdlfont(const char *filename) {
    egafont font;
    FILE *f = fopen(filename, "r");
    fread(&font, sizeof(egafont), 1, f);
    fclose(f);

    sdlfont *sfont = malloc(sizeof(*sfont));

    for (int i = 0; i < 256; ++i) {
        egachar c = font.charset[i];
        SDL_Texture *t = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 8, 14);
        SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
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

void render_sfont(sdlfont *sfont, unsigned short pair, int x, int y) {
    int bg = colors[(pair >> (8 + 4)) & 0x7],
        fg = colors[(pair >> 8) & 0xf],
        character = pair & 0xff;
    SDL_SetRenderDrawColor(renderer, bg >> 16, (bg >> 8) & 0xff, bg & 0xff, SDL_ALPHA_OPAQUE);
    SDL_Rect bgrect = { x * 8, y * 14, 8, 14 };
    SDL_RenderFillRect(renderer, &bgrect);

    SDL_Rect src = { 0, 0, 8, 14 };
    SDL_Rect dest = { x * 8, y * 14, 8, 14 };
    SDL_SetTextureColorMod(sfont->charset[character], fg >> 16, (fg >> 8) & 0xff, fg & 0xff);
    SDL_RenderCopy(renderer, sfont->charset[character], &src, &dest);
}

void free_sdlfont(sdlfont *sfont) {
    for (int i = 0; i < 256; ++i) {
        SDL_DestroyTexture(sfont->charset[i]);
    }

    free(sfont);
}

/* vim: set sw=4 et: */
