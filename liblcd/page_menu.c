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

#include "glib.h"
#include "page.h"

#include "page_menu.h"

static void menu_load(struct page_ctx *ctx, const struct page *page);
static void menu_free(struct page_ctx *ctx, const struct page *page);
static void menu_keydown(struct page_ctx *ctx, enum page_key key);

const struct page_type g_page_menu = {
    .load    = menu_load,
    .free    = menu_free,
    .keydown = menu_keydown
};

struct menu_ctx {
    uint16_t index;
    uint16_t window;
    uint16_t epp;
    uint16_t spacing;
    uint16_t fontsize;
};

static void menu_render(struct page_ctx *ctx)
{
    struct menu_ctx *menu = (struct menu_ctx*)ctx->page_ctx;
    const struct page *entry = (struct page*)(*ctx->top)->data + menu->window;
    struct glib_dev_info info;
    int i;

    /* Setup graphics environment */

    glib_clear(ctx->glib);
    page_setfont(ctx, PAGE_TYPE_MENU);

    glib_devinfo_get(ctx->glib, &info);

    int y = menu->fontsize;
    for (i = 0; i < menu->epp && entry->type != PAGE_TYPE_NONE; i++, entry++) {

        if ((menu->window + i) == menu->index) {

            glib_rect(ctx->glib, 0, y - menu->fontsize,
                      info.xres, menu->fontsize + menu->spacing, true);

            glib_enable(ctx->glib, GLIB_INVERT);
            glib_print(ctx->glib, 0, y, entry->title);
            glib_disable(ctx->glib, GLIB_INVERT);

        } else {
            glib_print(ctx->glib, 0, y, entry->title);
        }

        y += menu->fontsize + menu->spacing;
    }

    glib_flush(ctx->glib);
}

static void menu_load(struct page_ctx *ctx, const struct page *page)
{
    BUILD_BUG_ON(sizeof(struct menu_ctx) >= sizeof(ctx->page_ctx));

    struct menu_ctx *menu = (struct menu_ctx*)ctx->page_ctx;
    const struct glib_font *font;
    struct glib_dev_info info;

    ctx->ops->setfont(ctx, ctx->priv, PAGE_TYPE_MENU);
    font = glib_font_get(ctx->glib);

    glib_devinfo_get(ctx->glib, &info);

    /* Compute menu entries per page */

    menu->epp      = info.yres / font->size;
    menu->spacing  = (info.yres % font->size) / (menu->epp - 1);
    menu->fontsize = font->size;

    menu_render(ctx);
}

static void menu_free(struct page_ctx *ctx, const struct page *page)
{

}

static void menu_keydown(struct page_ctx *ctx, enum page_key key)
{
    struct menu_ctx *menu = (struct menu_ctx*)ctx->page_ctx;
    const struct page *entry = (struct page*)(*ctx->top)->data + menu->index;

    switch (key) {
    case PAGE_KEY_UP:
        if (menu->index > 0)
            menu->index--;
        break;

    case PAGE_KEY_DOWN:
        if (entry[1].type != PAGE_TYPE_NONE)
            menu->index++;
        break;

    case PAGE_KEY_OK:
        page_push(ctx, entry);
        return;

    case PAGE_KEY_BACK:
        page_pop(ctx);
        return;

    default:
        ;
    }

    if ((menu->index - menu->window) < 0) {
        menu->window--;
    } else if ((menu->index - menu->window) >= menu->epp) {
        menu->window++;
    }

    menu_render(ctx);
}
