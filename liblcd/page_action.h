#ifndef _LIBLCD_PAGE_ACTION_H_
#define _LIBLCD_PAGE_ACTION_H_

extern const struct page_type g_page_type_action;

#define PAGE_TYPE_ACTION &g_page_type_action

struct page_action {
    const char *text;
    void (*keydown)(struct page_ctx *ctx, void *priv, enum page_key key);
    void *data;
};


#endif  /* _LIBLCD_PAGE_ACTION_H_ */
