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

typedef struct {
    char *title;
    char is_immediate_window;

    doc_line_t *doc;
    int total_lines;

    int top, height;

    int cursor_x, cursor_y;
    int scroll_x, scroll_y;
} editor_t;

static editor_t *main_editor;

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

static doc_line_t *doc_line_alloc(void) {
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

static doc_line_t *get_current_doc_line(editor_t *editor) {
    doc_line_t *d = editor->doc;
    for (int y = 0; y < editor->cursor_y; ++y) {
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

static void split_line(editor_t *editor, doc_line_t *d, int offset) {
    doc_line_t *n = doc_line_alloc();
    n->next = d->next;
    n->prev = d;
    d->next = n;

    n->stored = d->stored - offset;
    ensure_available(n, n->stored);
    memcpy(n->line, d->line + offset, n->stored);

    d->stored -= n->stored;

    ++editor->total_lines;
}

static void delete_at(editor_t *editor, doc_line_t *d, int offset, int dir) {
    /* dir should be -1 (backspace) or 0 (delete) */
    if (dir == -1 && offset == 0) {
        doc_line_t *p = d->prev;

        if (!p) {
            /*  WRONG
             *   WAY
             * GO BACK */
            return;
        }

        --editor->cursor_y;
        editor->cursor_x = p->stored;

        ensure_available(p, d->stored);
        memcpy(p->line + p->stored, d->line, d->stored);
        p->next = d->next;
        p->stored += d->stored;
        if (d->next) {
            d->next->prev = p;
        }

        free_doc_line(d);
        --editor->total_lines;
    } else if (dir == -1) {
        /* offset > 0 */
        bcopy(d->line + offset, d->line + offset - 1, d->stored - offset);
        --d->stored;
        --editor->cursor_x;
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
        --editor->total_lines;
    } else {
        /* dir == 0, offset < d->stored */
        bcopy(d->line + offset + 1, d->line + offset, d->stored - offset - 1);
        --d->stored;
    }
}

static editor_t *editor_alloc(char const *title, int top, int height) {
    editor_t *editor = malloc(sizeof(*editor));
    memset(editor, 0, sizeof(*editor));
    editor->title = strdup(title);
    editor->doc = doc_line_alloc();
    editor->total_lines = 1;
    editor->top = top;
    editor->height = height;
    return editor;
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

    editor_t *editor = main_editor;

    /* Render the titlebar. */

    screen[editor->top * 80 + 0] = 0x17da;
    for (int x = 1; x < 79; ++x) {
        screen[editor->top * 80 + x] = 0x17c4;
    }

    int flen = strlen(editor->title);
    int start = 40 - flen / 2;
    screen[editor->top * 80 + start - 1] = 0x7000;

    int j;
    for (j = 0; j < flen; ++j) {
        screen[editor->top * 80 + start + j] = 0x7000 | editor->title[j];
    }

    screen[editor->top * 80 + start + j] = 0x7000;

    /* Render the little fullscreen widget at the right. */

    screen[editor->top * 80 + 75] = 0x17b4;
    screen[editor->top * 80 + 76] = 0x7112;
    screen[editor->top * 80 + 77] = 0x17c3;

    screen[editor->top * 80 + 79] = 0x17bf;

    /* Draw the editing area and borders. */

    for (int y = editor->top + 1; y < editor->top + 1 + editor->height; ++y) {
        screen[y * 80 + 0] = screen[y * 80 + 79] = 0x17b3;
        for (int x = 1; x < 79; ++x) {
            screen[y * 80 + x] = 0x1700;
        }
    }

    /* Render the editing text. */

    doc_line_t *line = main_editor->doc;
    for (int y = 0; y < main_editor->scroll_y && line; ++y, line = line->next) {}

    for (int y = 0; y < editor->height && line; ++y, line = line->next) {
        for (int x = main_editor->scroll_x; x < min(line->stored, 78 + main_editor->scroll_x); ++x) {
            screen[(y + editor->top + 1) * 80 + 1 + x - main_editor->scroll_x] += line->line[x];
        }
    }

    /* Draw the vertical scrollbar. */

    if (editor->height > 3) {
        screen[(editor->top + 1) * 80 + 79] = 0x7018;

        for (int y = editor->top + 2; y < editor->top + editor->height - 1; ++y) {
            screen[y * 80 + 79] = 0x70b0;
        }

        screen[(editor->top + 2 + (int)((float) main_editor->cursor_y / (main_editor->total_lines - 1) * (editor->height - 4))) * 80 + 79] = 0x0000;

        screen[(editor->top + editor->height - 1) * 80 + 79] = 0x7019;
    }

    /* Draw the horizontal scrollbar. */

    screen[(editor->top + editor->height) * 80 + 1] = 0x701b;

    for (int x = 2; x < 78; ++x) {
        screen[(editor->top + editor->height) * 80 + x] = 0x70b0;
    }
    screen[(editor->top + editor->height) * 80 + (int)((float) main_editor->scroll_x / 178 * 75) + 2] = 0x0000;

    screen[(editor->top + editor->height) * 80 + 78] = 0x701a;

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
    asprintf(&counter, "%05d:%03d", main_editor->cursor_y + 1, main_editor->cursor_x + 1);
    int len = strlen(counter);
    for (int i = 0; i < len; ++i) {
        screen[24 * 80 + 70 + i] += counter[i];
    }
    free(counter);

    /* Place the cursor. */

    screen_cursor_x = main_editor->cursor_x + 1 - main_editor->scroll_x;
    screen_cursor_y = main_editor->cursor_y + 2 - main_editor->scroll_y;
}

static void check_scroll(editor_t *editor) {
    if (editor->cursor_y < editor->scroll_y) {
        editor->scroll_y = editor->cursor_y;
    } else if (editor->cursor_y > editor->scroll_y + editor->height - 2) {
        editor->scroll_y = editor->cursor_y - editor->height + 2;
    }

    /* window width: 78 characters (1 to 78) */
    if (editor->cursor_x < editor->scroll_x) {
        editor->scroll_x = editor->cursor_x;
    } else if (editor->cursor_x > editor->scroll_x + 77) {
        editor->scroll_x = editor->cursor_x - 77;
    }
}

void qb_init(void) {
    main_editor = editor_alloc("Untitled", 1, 4);

    render();
}

void qb_keypress(SDL_Keycode sym, Uint16 mod) {
    if (sym == SDLK_ESCAPE) {
        qb_running = 0;
        return;
    }

    editor_t *editor = main_editor;

    if (sym == SDLK_DOWN && editor->cursor_y < editor->total_lines - 1) {
        ++editor->cursor_y;
        int max = get_current_doc_line(editor)->stored;
        if (editor->cursor_x > max) {
            editor->cursor_x = max;
        }
    } else if (sym == SDLK_UP && editor->cursor_y > 0) {
        --editor->cursor_y;
        int max = get_current_doc_line(editor)->stored;
        if (editor->cursor_x > max) {
            editor->cursor_x = max;
        }
    } else if (sym == SDLK_LEFT && editor->cursor_x > 0) {
        --editor->cursor_x;
    } else if (sym == SDLK_RIGHT) {
        if (editor->cursor_x < get_current_doc_line(editor)->stored) {
            ++editor->cursor_x;
        }
    } else if (is_printable_key(sym) && get_current_doc_line(editor)->stored < 255) {
        insert_character(
            get_current_doc_line(editor),
            editor->cursor_x,
            get_character(sym, mod));
        ++editor->cursor_x;
    } else if (sym == SDLK_RETURN) {
        split_line(editor, get_current_doc_line(editor), editor->cursor_x);
        editor->cursor_x = 0;
        ++editor->cursor_y;
    } else if (sym == SDLK_BACKSPACE) {
        delete_at(editor, get_current_doc_line(editor), editor->cursor_x, -1);
    } else if (sym == SDLK_DELETE) {
        delete_at(editor, get_current_doc_line(editor), editor->cursor_x, 0);
    } else if (sym == SDLK_HOME) {
        editor->cursor_x = 0;
    } else if (sym == SDLK_END) {
        editor->cursor_x = get_current_doc_line(editor)->stored;
    }

    check_scroll(editor);
    render();
    text_refresh();
}

/* vim: set sw=4 et: */
