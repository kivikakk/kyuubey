#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qb.h"
#include "text.h"

int qb_running = 1;

typedef struct doc_line {
    char *line;
    int allocated, stored;
    struct doc_line *prev, *next;
} doc_line_t;

static doc_line_t *active_doc;
static int total_lines = 1;

static int cursor_x = 0;
static int cursor_y = 0;

static int scroll_x = 0;
static int scroll_y = 0;

static SDL_Keycode shift_table[][2] = {
    {SDLK_QUOTE, SDLK_QUOTEDBL},
    {SDLK_COMMA, SDLK_LESS},
    {SDLK_MINUS, SDLK_UNDERSCORE},
    {SDLK_PERIOD, SDLK_GREATER},
    {SDLK_SLASH, SDLK_QUESTION},
    {SDLK_0, SDLK_RIGHTPAREN},
    {SDLK_1, SDLK_EXCLAIM},
    {SDLK_2, SDLK_AT},
    {SDLK_3, SDLK_HASH},
    {SDLK_4, SDLK_DOLLAR},
    {SDLK_5, SDLK_PERCENT},
    {SDLK_6, SDLK_CARET},
    {SDLK_7, SDLK_AMPERSAND},
    {SDLK_8, SDLK_ASTERISK},
    {SDLK_9, SDLK_LEFTPAREN},
    {SDLK_SEMICOLON, SDLK_COLON},
    {SDLK_LEFTBRACKET, '{'},
    {SDLK_BACKSLASH, '|'},
    {SDLK_RIGHTBRACKET, '}'},
    {SDLK_BACKQUOTE, '~'},
    {SDLK_EQUALS, SDLK_PLUS},
};

static int min(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

static doc_line_t *create_doc_line(void) {
    doc_line_t *d = malloc(sizeof(*d));
    memset(d, 0, sizeof(*d));
    d->line = malloc(8);
    d->allocated = 8;
    return d;
}

static void free_doc_line(doc_line_t *d) {
    free(d->line);
    free(d);
}

static doc_line_t *get_current_doc_line(void) {
    doc_line_t *d = active_doc;
    for (int y = 0; y < cursor_y; ++y) {
        d = d->next;
    }
    return d;
}

static void ensure_available(doc_line_t *d, int required) {
    int allocated = d->allocated;
    while (allocated < d->stored + required) {
        allocated *= 2;
    }

    if (allocated == d->allocated) {
        return;
    }

    char *line = malloc(allocated);
    memcpy(line, d->line, d->stored);
    free(d->line);
    d->line = line;
    d->allocated = allocated;
}

static void insert_character(doc_line_t *d, int offset, char c) {
    ensure_available(d, 1);
    bcopy(d->line + offset, d->line + offset + 1, d->stored - offset);
    d->line[offset] = c;
    ++d->stored;
}

static void split_line(doc_line_t *d, int offset) {
    doc_line_t *n = create_doc_line();
    n->next = d->next;
    n->prev = d;
    d->next = n;

    n->stored = d->stored - offset;
    ensure_available(n, n->stored);
    memcpy(n->line, d->line + offset, n->stored);

    d->stored -= n->stored;

    ++total_lines;
}

static void delete_at(doc_line_t *d, int offset, int dir) {
    /* dir should be -1 (backspace) or 0 (delete) */
    if (dir == -1 && offset == 0) {
        doc_line_t *p = d->prev;

        if (!p) {
            /*  WRONG
             *   WAY
             * GO BACK */
            return;
        }

        --cursor_y;
        cursor_x = p->stored;

        ensure_available(p, d->stored);
        memcpy(p->line + p->stored, d->line, d->stored);
        p->next = d->next;
        p->stored += d->stored;
        if (d->next) {
            d->next->prev = p;
        }

        free_doc_line(d);
        --total_lines;
    } else if (dir == -1) {
        /* offset > 0 */
        bcopy(d->line + offset, d->line + offset - 1, d->stored - offset);
        --d->stored;
        --cursor_x;
    } else if (offset == d->stored) {
        /* dir == 0 */
        doc_line_t *n = d->next;

        if (!n) {
            return;
        }

        ensure_available(d, n->stored);
        memcpy(d->line + d->stored, n->line, n->stored);
        d->stored += n->stored;

        d->next = n->next;
        if (n->next) {
            n->next->prev = d;
        }

        free_doc_line(n);
        --total_lines;
    } else {
        /* dir == 0, offset < d->stored */
        bcopy(d->line + offset + 1, d->line + offset, d->stored - offset - 1);
        --d->stored;
    }
}

static char get_character(SDL_Keycode sym, Uint16 mod) {
    if (sym >= SDLK_a && sym <= SDLK_z) {
        if (mod & (KMOD_SHIFT | KMOD_CAPS)) {
            return (char) (sym - ('a' - 'A'));
        }
        return (char) sym;
    }

    if (mod & KMOD_SHIFT) {
        for (int i = 0; i < sizeof(shift_table); ++i) {
            if (shift_table[i][0] == sym) {
                return shift_table[i][1];
            }
        }
    }

    return (char) sym;
}

static int is_printable_key(SDL_Keycode sym) {
    return sym >= SDLK_SPACE && sym <= SDLK_z;
}

void render(void) {
    for (int i = 0; i < 80 * 25; ++i) {
        screen[i] = 0x1700;
    }

    /* Render the menu. */
    
    for (int x = 0; x < 80; ++x) {
        screen[x] = 0x7000;
    }

    const char *menu_options[] = {
        "File",
        "Edit",
        "View",
        "Search",
        "Run",
        "Debug",
        "Calls",
        "Options",
        NULL,
    };

    int offset = 2;
    for (int i = 0; ; ++i) {
        if (!menu_options[i]) {
            break;
        }

        int len = strlen(menu_options[i]);
        for (int j = 0; j < len; ++j) {
            screen[0 * 80 + offset + 1 + j] = 0x7000 | (menu_options[i][j]);
        }

        offset += len + 2;
    }

    screen[0 * 80 + 74] = 0x7000 + 'H';
    screen[0 * 80 + 75] = 0x7000 + 'e';
    screen[0 * 80 + 76] = 0x7000 + 'l';
    screen[0 * 80 + 77] = 0x7000 + 'p';

    /* Render the titlebar. */

    screen[1 * 80 + 0] = 0x17da;
    for (int x = 1; x < 79; ++x) {
        screen[1 * 80 + x] = 0x17c4;
    }

    const char *file = "Untitled";
    int flen = strlen(file);
    int start = 40 - flen / 2;
    screen[1 * 80 + start - 1] = 0x7000;

    int j;
    for (j = 0; j < flen; ++j) {
        screen[1 * 80 + start + j] = 0x7000 | file[j];
    }

    screen[1 * 80 + start + j] = 0x7000;

    /* Render the little fullscreen widget at the right. */

    screen[1 * 80 + 75] = 0x17b4;
    screen[1 * 80 + 76] = 0x7112;
    screen[1 * 80 + 77] = 0x17c3;

    screen[1 * 80 + 79] = 0x17bf;

    /* Draw the editing area and borders. */

    for (int y = 2; y < 24; ++y) {
        screen[y * 80 + 0] = screen[y * 80 + 79] = 0x17b3;
        for (int x = 1; x < 79; ++x) {
            screen[y * 80 + x] = 0x1700;
        }
    }

    /* Render the editing text. */

    doc_line_t *line = active_doc;
    for (int y = 0; y < scroll_y && line; ++y, line = line->next) {}

    for (int y = 0; y < 21 && line; ++y, line = line->next) {
        for (int x = scroll_x; x < min(line->stored, 78 + scroll_x); ++x) {
            screen[(y + 2) * 80 + 1 + x - scroll_x] += line->line[x];
        }
    }

    /* Draw the vertical scrollbar. */

    screen[2 * 80 + 79] = 0x7018;

    for (int y = 3; y < 22; ++y) {
        screen[y * 80 + 79] = 0x70b0;
    }

    screen[(3 + (int)((float) cursor_y / (total_lines - 1) * (21 - 3))) * 80 + 79] = 0x0000;

    screen[22 * 80 + 79] = 0x7019;

    /* Draw the horizontal scrollbar. */

    screen[23 * 80 + 1] = 0x701b;

    for (int x = 2; x < 78; ++x) {
        screen[23 * 80 + x] = 0x70b0;
    }
    screen[23 * 80 + (int)((float) scroll_x / 178 * 75) + 2] = 0x0000;

    screen[23 * 80 + 78] = 0x701a;

    /* Draw the help line. */

    const char *footer[] = {
        "Shift+F1=Help",
        "F6=Window",
        "F2=Subs",
        "F5=Run",
        "F8=Step",
        NULL,
    };

    for (int x = 0; x < 80; ++x) {
        screen[24 * 80 + x] = 0x3000;
    }

    offset = 1;
    for (int i = 0; ; ++i) {
        if (!footer[i]) {
            break;
        }

        int len = strlen(footer[i]);
        screen[24 * 80 + offset] += '<';
        int j;
        for (j = 0; j < len; ++j) {
            screen[24 * 80 + offset + 1 + j] += footer[i][j];
        }
        screen[24 * 80 + offset + 1 + j] += '>';
        
        offset += len + 3;
    }

    /* Draw the ruler. */

    screen[24 * 80 + 62] += 0xb3;

    char *counter;
    asprintf(&counter, "%05d:%03d", cursor_y + 1, cursor_x + 1);
    int len = strlen(counter);
    for (int i = 0; i < len; ++i) {
        screen[24 * 80 + 70 + i] += counter[i];
    }
    free(counter);

    /* Place the cursor. */

    screen_cursor_x = cursor_x + 1 - scroll_x;
    screen_cursor_y = cursor_y + 2 - scroll_y;
}

static void check_scroll(void) {
    /* window height: 21 lines (2 to 22) */
    if (cursor_y < scroll_y) {
        scroll_y = cursor_y;
    } else if (cursor_y > scroll_y + 20) {
        scroll_y = cursor_y - 20;
    }

    /* window width: 78 characters (1 to 78) */
    if (cursor_x < scroll_x) {
        scroll_x = cursor_x;
    } else if (cursor_x > scroll_x + 77) {
        scroll_x = cursor_x - 77;
    }
}

void qb_init(void) {
    active_doc = create_doc_line();
    active_doc->line = strdup("10 PRINT \"LOL\"");
    active_doc->stored = strlen(active_doc->line);
    active_doc->allocated = active_doc->stored + 1;

    active_doc->next = create_doc_line();
    active_doc->next->prev = active_doc;
    active_doc->next->line = strdup("20 GOTO 10");
    active_doc->next->stored = strlen(active_doc->next->line);
    active_doc->next->allocated = active_doc->next->stored + 1;

    active_doc->next->next = create_doc_line();
    active_doc->next->next->prev = active_doc->next;

    total_lines = 3;

    render();
}

void qb_keypress(SDL_Keycode sym, Uint16 mod) {
    if (sym == SDLK_ESCAPE) {
        qb_running = 0;
        return;
    }

    if (sym == SDLK_DOWN && cursor_y < total_lines - 1) {
        ++cursor_y;
        int max = get_current_doc_line()->stored;
        if (cursor_x > max) {
            cursor_x = max;
        }
    } else if (sym == SDLK_UP && cursor_y > 0) {
        --cursor_y;
        int max = get_current_doc_line()->stored;
        if (cursor_x > max) {
            cursor_x = max;
        }
    } else if (sym == SDLK_LEFT && cursor_x > 0) {
        --cursor_x;
    } else if (sym == SDLK_RIGHT) {
        if (cursor_x < get_current_doc_line()->stored) {
            ++cursor_x;
        }
    } else if (is_printable_key(sym)) {
        insert_character(
            get_current_doc_line(),
            cursor_x,
            get_character(sym, mod));
        ++cursor_x;
    } else if (sym == SDLK_RETURN) {
        split_line(get_current_doc_line(), cursor_x);
        cursor_x = 0;
        ++cursor_y;
    } else if (sym == SDLK_BACKSPACE) {
        delete_at(get_current_doc_line(), cursor_x, -1);
    } else if (sym == SDLK_DELETE) {
        delete_at(get_current_doc_line(), cursor_x, 0);
    } else if (sym == SDLK_HOME) {
        cursor_x = 0;
    } else if (sym == SDLK_END) {
        cursor_x = get_current_doc_line()->stored;
    }

    check_scroll();
    render();
    text_refresh();
}

/* vim: set sw=4 et: */
