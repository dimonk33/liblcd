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

void menu_init(struct menu_ctx *ctx, struct glib_ctx *glib, void *priv,
               struct menu_page *root)
{
    memset(ctx, 0, sizeof(*ctx));

    ctx->glib = glib;
    ctx->priv = priv;

    /* init page stack */
    ctx->top = ctx->stack - 1;

    menu_page_push(ctx, root);
}

void menu_process(struct menu_ctx *ctx)
{
    (void)ctx;
}

void menu_up(struct menu_ctx *ctx)
{
    (void)ctx;
}

void menu_down(struct menu_ctx *ctx)
{
    (void)ctx;
}

void menu_ok(struct menu_ctx *ctx)
{
    (void)ctx;
}

void menu_back(struct menu_ctx *ctx)
{
    (void)ctx;
}

void menu_page_push(struct menu_ctx *ctx, struct menu_page *page)
{
    assert(*ctx->top < ctx->stack[CONFIG_MENU_STACKSIZE]);
    *++ctx->top = page;
}

void menu_page_pop(struct menu_ctx *ctx)
{
    if (ctx->top > ctx->stack) {
        ctx->top--;
    }
}
