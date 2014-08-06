/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 UVC Ingenieure http://uvc.de/
 * Author: Max Holtzberg <mh@uvc.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _LIBLCD_MENU_H_
#define _LIBLCD_MENU_H_

#include "config.h"

#define MENU_SIZE(x) (sizeof((x)) / sizeof((x)[0]))

/* Forward declarations */
struct glib_ctx;
struct menu_ctx;
struct menu_page;

struct menu_page_menu {
    int len;
    const struct menu_page *entries;
};

struct menu_page_menu_ctx {
    int index;                  /*!< Selected menu entry */
    int window;                 /*!< Window position */
    int epp;                    /*!< Entries per page */
};

struct menu_page_text {
    void (*get)(char *text);
};

enum menu_page_type {
    MENU_PAGE_MENU,
    MENU_PAGE_TEXT,
    MENU_PAGE_INVALID
};

struct menu_page {
    enum menu_page_type type;
    const char *title;
    union {
        struct menu_page_menu menu;
        struct menu_page_text text;
    } u;
};

struct menu_ops {
    void (*setfont)(struct menu_ctx *ctx, void *priv, enum menu_page_type type);
};

struct menu_ctx {
    struct glib_ctx *glib;
    const struct menu_ops *ops;
    void *priv;
    const struct menu_page *stack[CONFIG_MENU_STACKSIZE];
    const struct menu_page **top;
    union {
        struct menu_page_menu_ctx menu;
    } u;
};

enum menu_action {
    MENU_UP,
    MENU_DOWN,
    MENU_BACK,
    MENU_OK
};

void menu_init(struct menu_ctx *ctx, struct glib_ctx *glib, void *priv,
               const struct menu_ops *ops, const struct menu_page *root);
void menu_process(struct menu_ctx *ctx);
void menu_render(struct menu_ctx *ctx);

void menu_do(struct menu_ctx *ctx, enum menu_action action);

void menu_page_push(struct menu_ctx *ctx, const struct menu_page *page);
void menu_page_pop(struct menu_ctx *ctx);

#endif  /* _LIBLCD_MENU_H_ */
