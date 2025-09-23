#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "chipeu.h"

#ifndef ANDROID_EXPORT
#define ANDROID_EXPORT __attribute__((visibility("default")))
#endif

static chipeu chip;
static uint8_t *rom_buf = nullptr;
static int rom_size = 0;

extern "C"
{

    ANDROID_EXPORT void loadRom(const uint8_t *data, int size)
    {
        if (rom_buf)
        {
            free(rom_buf);
            rom_buf = nullptr;
            rom_size = 0;
        }
        if (!data || size <= 0)
            return;
        rom_buf = (uint8_t *)malloc((size_t)size);
        if (!rom_buf)
            return;
        memcpy(rom_buf, data, (size_t)size);
        rom_size = size;
        // Adjust cast if core expects bytes differently
        chip.loadRom((const char *)rom_buf);
    }

    ANDROID_EXPORT void freeRom()
    {
        if (rom_buf)
        {
            free(rom_buf);
            rom_buf = nullptr;
            rom_size = 0;
        }
    }

    ANDROID_EXPORT void emulateCycle()
    {
        chip.emulateCycle();
    }

    ANDROID_EXPORT const uint8_t *getGfx()
    {
        return chip.getGfx();
    }

    ANDROID_EXPORT bool getDraw()
    {
        return chip.getDraw();
    }

    ANDROID_EXPORT void setDraw()
    {
        chip.setDraw(false);
    }
}
