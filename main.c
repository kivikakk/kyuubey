#include "main.h"
#include "parser.h"
#include "qb.h"
#include "renderer.h"
#include "text.h"

int mouse_x = 0;
int mouse_y = 0;

static void loop(void);

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "parser-test") == 0) {
        return parser_test();
    }

    int r = renderer_init();
    if (r) {
        return r;
    }

    qb_init();

    loop();

    renderer_quit();

    return 0;
}

#define FPS 60  /* Technically: vsync rate */
#define TICK_LENGTH (1000 / FPS)
#define FLIP_CURSOR 16  /* frame count */

#define TYPEMATIC_DELAY 250
#define TYPEMATIC_REPEAT 100

void loop(void) {
    SDL_Event event;
    Uint32 last_tick = SDL_GetTicks();

    text_refresh();
    int until_flip = FLIP_CURSOR;

    Uint32 keydown_tick = 0;
    SDL_Keycode keydown_sym;
    Uint16 keydown_mod;
    int typematic_on = 0;

    while (qb_running) {
        if (keydown_tick && !typematic_on && last_tick >= keydown_tick + TYPEMATIC_DELAY) {
            typematic_on = 1;
            keydown_tick = last_tick;
            qb_keypress(keydown_sym, keydown_mod);
        } else if (keydown_tick && typematic_on && last_tick >= keydown_tick + TYPEMATIC_REPEAT) {
            keydown_tick = last_tick;
            qb_keypress(keydown_sym, keydown_mod);
        }

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                return;

            case SDL_KEYDOWN:
                if (event.key.repeat) {
                    break;
                }

                qb_keydown(event.key.keysym.sym, event.key.keysym.mod);
                qb_keypress(event.key.keysym.sym, event.key.keysym.mod);
                keydown_tick = SDL_GetTicks();
                keydown_sym = event.key.keysym.sym;
                keydown_mod = event.key.keysym.mod;
                typematic_on = 0;
                break;

            case SDL_KEYUP:
                qb_keyup(event.key.keysym.sym);
                keydown_tick = 0;
                break;

            case SDL_MOUSEMOTION: {
                int old_x = mouse_x,
                    old_y = mouse_y;
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;

                if (mouse_x != old_x || mouse_y != old_y) {
                    text_refresh();
                }
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
                mouse_x = event.button.x;
                mouse_y = event.button.y;
                text_refresh();
                if (event.button.button == SDL_BUTTON_LEFT ||
                        event.button.button == SDL_BUTTON_RIGHT) {
                    qb_mouseclick(event.button.button == SDL_BUTTON_LEFT ?  1 : 2);
                }
                break;
            }
        }

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
            text_refresh();
        }
    }
}

/* vim: set sw=4 et: */
