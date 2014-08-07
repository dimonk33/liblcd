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

#ifndef _LIBLCD_PAGE_H_
#define _LIBLCD_PAGE_H_

#include <stdint.h>

#include "config.h"

#define PAGE_TYPE_NONE NULL

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

struct page;
struct page_ctx;

enum page_key {
    PAGE_KEY_UP,
    PAGE_KEY_DOWN,
    PAGE_KEY_OK,
    PAGE_KEY_BACK
};

struct page_type {
    void (*load)(struct page_ctx *ctx, const struct page *page);
    void (*free)(struct page_ctx *ctx, const struct page *page);
    void (*keydown)(struct page_ctx *ctx, enum page_key key);
};

typedef const struct page_type* page_type_t;

struct page {
    const char *title;
    const struct page_type *type;
    const void *data;
};

struct page_ops {
    void (*setfont)(struct page_ctx *ctx, void *priv, page_type_t type);
};

struct page_ctx {
    struct glib_ctx *glib;
    const struct page_ops *ops;
    void *priv;
    const struct page *stack[CONFIG_PAGE_STACKSIZE];
    const struct page **top;
    uint8_t page_ctx[CONFIG_PAGE_CONTEXTSIZE];
};


void page_init(struct page_ctx *ctx, struct glib_ctx *glib, void *priv,
               const struct page_ops *ops, const struct page *root);
void page_push(struct page_ctx *ctx, const struct page *page);
void page_pop(struct page_ctx *ctx);
void page_setfont(struct page_ctx *ctx, page_type_t type);
void page_keydown(struct page_ctx *ctx, enum page_key key);


#endif  /* _LIBLCD_PAGE_H_ */
