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

#include "page_action.h"

static void action_load(struct page_ctx *ctx, const struct page *page);
static void action_free(struct page_ctx *ctx, const struct page *page);
static void action_keydown(struct page_ctx *ctx, enum page_key key);

const struct page_type g_page_type_action = {
    .load    = action_load,
    .free    = action_free,
    .keydown = action_keydown
};

static void action_load(struct page_ctx *ctx, const struct page *page)
{
    const struct page_action *action = (*ctx->top)->data;
    const struct glib_font *font;

    /* Setup graphics environment */

    glib_clear(ctx->glib);
    page_setfont(ctx, PAGE_FONT_SMALL);

    ctx->ops->setfont(ctx, ctx->priv, PAGE_FONT_SMALL);
    font = glib_font_get(ctx->glib);

    glib_print(ctx->glib, 0, font->size, page->title);

    glib_print(ctx->glib, 0, 2.5 * font->size, action->text);
    glib_flush(ctx->glib);
}

static void action_free(struct page_ctx *ctx, const struct page *page)
{

}

static void action_keydown(struct page_ctx *ctx, enum page_key key)
{
    const struct page_action *action = (*ctx->top)->data;
    action->keydown(ctx, action->data, key);
}
