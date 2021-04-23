#include "sfont.h"
#include "renderer.h"

/* adorable CGA: https://en.wikipedia.org/wiki/Color_Graphics_Adapter#Color_palette */
int cga_colors[16] = {
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
    unsigned char bitmap[FONT_HEIGHT];
} rawchar_t;

typedef struct {
    rawchar_t charset[256];
} rawfont_t;

sdlfont_t *read_raw_sdlfont(const char *filename) {
    rawfont_t font;
    FILE *f = fopen(filename, "r");
    fread(&font, sizeof(rawfont_t), 1, f);
    fclose(f);

    sdlfont_t *sfont = malloc(sizeof(*sfont));

    for (int i = 0; i < 256; ++i) {
        rawchar_t c = font.charset[i];
        SDL_Texture *t = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, FONT_WIDTH, FONT_HEIGHT);
        SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, t);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);

        for (int row = 0; row < FONT_HEIGHT; ++row) {
            unsigned char rowdata = c.bitmap[row],
                          mask = 0x80;
            for (int offset = 0; offset < FONT_WIDTH; ++offset) {
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

void render_sfont(sdlfont_t *sfont, unsigned short pair, int x, int y) {
    int bg = cga_colors[(pair >> (8 + 4)) & 0x7],
        fg = cga_colors[(pair >> 8) & 0xf],
        character = pair & 0xff;
    SDL_SetRenderDrawColor(renderer, bg >> 16, (bg >> 8) & 0xff, bg & 0xff, SDL_ALPHA_OPAQUE);
    SDL_Rect bgrect = { x * FONT_WIDTH, y * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT };
    SDL_RenderFillRect(renderer, &bgrect);

    SDL_Rect src = { 0, 0, FONT_WIDTH, FONT_HEIGHT };
    SDL_Rect dest = { x * FONT_WIDTH, y * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT };
    SDL_SetTextureColorMod(sfont->charset[character], fg >> 16, (fg >> 8) & 0xff, fg & 0xff);
    SDL_RenderCopy(renderer, sfont->charset[character], &src, &dest);
}

void free_sdlfont(sdlfont_t *sfont) {
    for (int i = 0; i < 256; ++i) {
        SDL_DestroyTexture(sfont->charset[i]);
    }

    free(sfont);
}

/* vim: set sw=4 et: */
