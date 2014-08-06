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

#include <assert.h>
#include <string.h>

#include "glib.h"
#include "menu.h"

#include <debug.h>

#ifndef ABS
#  define ABS(x) ((x) < 0 ? -(x) : (x))
#endif

static void render_menu(struct menu_ctx *ctx)
{
    struct menu_page_menu_ctx *menu = &ctx->u.menu;
    const struct glib_font *font;
    const struct menu_page *entry;
    struct glib_dev_info info;
    int i;

    ctx->ops->setfont(ctx, ctx->priv, MENU_PAGE_MENU);
    font = glib_font_get(ctx->glib);

    glib_devinfo_get(ctx->glib, &info);

    /* Compute menu entries per page */

    menu->epp   = info.yres / font->size;
    int spacing = (info.yres % font->size) / (menu->epp - 1);
    dbg("Entries per page: %d spacing: %d\r\n", menu->epp, spacing);

    /* Get current entry */

    entry = &(*ctx->top)->u.menu.entries[menu->window];

    glib_clear(ctx->glib);
    int y = font->size;
    for (i = 0; i < menu->epp && entry->type != MENU_PAGE_INVALID; i++, entry++) {

        if ((menu->window + i) == menu->index) {

            glib_rect(ctx->glib, 0, y - font->size,
                      info.xres, font->size + spacing, true);

            glib_enable(ctx->glib, GLIB_INVERT);
            glib_print(ctx->glib, 0, y, entry->title);
            glib_disable(ctx->glib, GLIB_INVERT);

        } else {
            glib_print(ctx->glib, 0, y, entry->title);
        }

        y += font->size + spacing;
    }

    glib_flush(ctx->glib);
}

static void render_text(struct menu_ctx *ctx)
{

}

void menu_init(struct menu_ctx *ctx, struct glib_ctx *glib, void *priv,
               const struct menu_ops *ops, const struct menu_page *root)
{
    memset(ctx, 0, sizeof(*ctx));

    ctx->glib = glib;
    ctx->priv = priv;
    ctx->ops  = ops;

    /* init page stack */
    ctx->top = ctx->stack - 1;

    menu_page_push(ctx, root);
    menu_render(ctx);
}

void menu_process(struct menu_ctx *ctx)
{
    (void)ctx;
}

void menu_render(struct menu_ctx *ctx)
{
    switch ((*ctx->top)->type) {
    case MENU_PAGE_MENU:
        render_menu(ctx);
        break;

    case MENU_PAGE_TEXT:
        render_text(ctx);
        break;

    default:
        ;
    }
}

void menu_do(struct menu_ctx *ctx, enum menu_action action)
{
    const struct menu_page_menu *menu_page = &(*ctx->top)->u.menu;
    struct menu_page_menu_ctx *menu_ctx = &ctx->u.menu;

    switch ((*ctx->top)->type) {
    case MENU_PAGE_MENU:
        if (action == MENU_UP) {
            if (menu_ctx->index > 0)
                menu_ctx->index--;

        } else if (action == MENU_DOWN) {
            if (menu_ctx->index < (menu_page->len - 1))
                ctx->u.menu.index++;
        }

        if ((menu_ctx->index - menu_ctx->window) < 0) {
            menu_ctx->window--;
        } else if ((menu_ctx->index - menu_ctx->window) >= menu_ctx->epp) {
            menu_ctx->window++;
        }

        break;

    }

    menu_render(ctx);
}

void menu_page_push(struct menu_ctx *ctx, const struct menu_page *page)
{
    assert(*ctx->top < ctx->stack[CONFIG_MENU_STACKSIZE]);
    *++ctx->top = page;
    memset(&ctx->u, 0, sizeof(ctx->u));
}

void menu_page_pop(struct menu_ctx *ctx)
{
    if (ctx->top > ctx->stack) {
        ctx->top--;
    }
}
