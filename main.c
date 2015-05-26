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

        SDL_Delay(10);
    }
}

/* vim: set sw=4 et: */
