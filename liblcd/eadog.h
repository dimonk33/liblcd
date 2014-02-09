#ifndef DOGM_H_
#define DOGM_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "glib.h"

#define CONFIG_EADOG_M132


/* Number        | Type           | Controller
 * -----------------------------------------
 * EA DOGS102-6  | 102x64  - 1,7" | UC1701
 * EA DOGM132-5  | 132x32  - 2,1" | ST7565
 * EA DOGM128-6  | 128x64  - 2,3" | ST7565
 * EA DOGM240-6  | 240x64  - 3,6" | UC1611s
 * EA DOGL128-6  | 128x64  - 2,8" | ST7565
 * EA DOGXL160-7 | 160x104 - 3,3" | UC1610
 * EA DOGXL240-7 | 240x128 - 3,9" | UC1611s
 */

#if defined(CONFIG_EADOG_S102)
#  define EADOG_XRES 102
#  define EADOG_YRES 64
#elif defined(CONFIG_EADOG_M132)
#  define EADOG_XRES 132
#  define EADOG_YRES 32
#elif defined(CONFIG_EADOG_M128)
#  define EADOG_XRES 128
#  define EADOG_YRES 64
#elif defined(CONFIG_EADOG_M240)
#  define EADOGM_XRES 240
#  define EADOGM_YRES 64
#elif defined(CONFIG_EADOG_XL160)
#  define EADOGM_XRES 240
#  define EADOGM_YRES 104
#elif defined(CONFIG_EADOG_XL240)
#  define EADOGM_XRES 240
#  define EADOGM_YRES 128
#else
#  error "Display type not defined or unkown"
#endif

#define EADOG_NPAGES (EADOG_YRES / 8)

#define DELAY(l) do {         \
    int i;                    \
    for (i = 0; i < l; i++) \
        __asm__("nop");       \
    } while(0)


struct eadog {
    struct glib_lcd lcd;

    void (*reset)(void *priv, bool enable);
    void (*data)(void *priv, bool enable);
    void (*write)(void *priv, const uint8_t *data, size_t len);
    void *priv;

    uint8_t dirty;        /* Bitmask to mark dirty pages */
    uint8_t fb[EADOG_NPAGES][EADOG_XRES];
};


void eadog_init(struct eadog *dev, void *priv);


#endif  /* DOGM_H_ */
