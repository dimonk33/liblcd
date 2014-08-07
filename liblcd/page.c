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

#include "page.h"

void page_init(struct page_ctx *ctx, struct glib_ctx *glib, void *priv,
               const struct page_ops *ops, const struct page *root)
{
    memset(ctx, 0, sizeof(*ctx));

    ctx->glib = glib;
    ctx->priv = priv;
    ctx->ops  = ops;

    /* init page stack */
    ctx->top = ctx->stack - 1;

    page_push(ctx, root);
}

void page_push(struct page_ctx *ctx, const struct page *page)
{
    assert(*ctx->top < ctx->stack[CONFIG_PAGE_STACKSIZE]);
    *++ctx->top = page;
    page->type->load(ctx, page);
}

void page_pop(struct page_ctx *ctx)
{
    if (ctx->top > ctx->stack) {
        (*ctx->top)->type->free(ctx, *ctx->top);
        ctx->top--;
    }
}

void page_setfont(struct page_ctx *ctx, page_type_t type)
{
    ctx->ops->setfont(ctx, ctx->priv, type);
}

void page_keydown(struct page_ctx *ctx, enum page_key key)
{
    (*ctx->top)->type->keydown(ctx, key);
}
