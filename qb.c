#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qb.h"
#include "text.h"

doc_line_t *active_doc;
int cursor_x = 0;
int cursor_y = 0;
int total_lines = 1;

static doc_line_t *create_doc_line(void) {
    doc_line_t *doc_line = malloc(sizeof(*doc_line));
    memset(doc_line, 0, sizeof(*doc_line));
    doc_line->line = malloc(8);
    doc_line->allocated = 8;
    return doc_line;
}

void qb_init(void) {
    active_doc = create_doc_line();
    active_doc->line = strdup("10 PRINT \"LOL\"");
    active_doc->stored = strlen(active_doc->line);
    active_doc->allocated = active_doc->stored + 1;

    active_doc->next = create_doc_line();
    active_doc->next->line = strdup("20 GOTO 10");
    active_doc->next->stored = strlen(active_doc->next->line);
    active_doc->next->allocated = active_doc->next->stored + 1;

    total_lines = 2;

    qb_render();
}

void qb_render(void) {
    for (int i = 0; i < 80 * 25; ++i) {
        screen[i] = 0x1700;
    }
    
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

    screen[1 * 80 + 75] = 0x17b4;
    screen[1 * 80 + 76] = 0x7112;
    screen[1 * 80 + 77] = 0x17c3;

    screen[1 * 80 + 79] = 0x17bf;

    for (int y = 2; y < 24; ++y) {
        screen[y * 80 + 0] = screen[y * 80 + 79] = 0x17b3;
        for (int x = 1; x < 79; ++x) {
            screen[y * 80 + x] = 0x1700;
        }
    }

    doc_line_t *line = active_doc;
    for (int y = 0; y < 22 && line; ++y, line = line->next) {
        for (int x = 0; x < line->stored; ++x) {
            screen[(y + 2) * 80 + 1 + x] += line->line[x];
        }
    }

    screen[2 * 80 + 79] = 0x7018;
    screen[3 * 80 + 79] = 0x0000;
    for (int y = 4; y < 22; ++y) {
        screen[y * 80 + 79] = 0x70b0;
    }
    screen[22 * 80 + 79] = 0x7019;

    screen[23 * 80 + 1] = 0x701b;
    screen[23 * 80 + 2] = 0x0000;
    for (int x = 3; x < 78; ++x) {
        screen[23 * 80 + x] = 0x70b0;
    }
    screen[23 * 80 + 78] = 0x701a;

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

    screen[24 * 80 + 62] += 0xb3;

    char *counter;
    asprintf(&counter, "%05d:%03d", cursor_y + 1, cursor_x + 1);
    int len = strlen(counter);
    for (int i = 0; i < len; ++i) {
        screen[24 * 80 + 70 + i] += counter[i];
    }
    free(counter);

    screen_cursor_x = cursor_x + 1;
    screen_cursor_y = cursor_y + 2;
}

/* vim: set sw=4 et: */
