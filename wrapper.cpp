/*

Author:
    KARTAVYA SHUKLA https://github.com/Novfensec
LICENSE:
    MIT - Copyright 2025 KARTAVYA SHUKLA

*/

#include "chipeu.h"
#include <stdlib.h>
#include <memory.h>

static chipeu chip;
unsigned char *rom = nullptr;
int romSize = 0;

extern "C"
{

    __declspec(dllexport) void loadRom(const char *data, int size)
    {
        if (rom)
        {
            free(rom);
            rom = nullptr;
        }

        rom = (unsigned char *)malloc(size);
        if (rom)
        {
            memcpy(rom, data, size);
            romSize = size;
            chip.loadRom(data);
        }
    }

    __declspec(dllexport) void freeRom()
    {
        if (rom)
        {
            free(rom);
            rom = nullptr;
            romSize = 0;
        }
    }

    __declspec(dllexport) void emulateCycle()
    {
        chip.emulateCycle();
    }

    __declspec(dllexport) const unsigned char *getGfx()
    {
        return chip.getGfx();
    }

    __declspec(dllexport) bool getDraw()
    {
        return chip.getDraw();
    }

    __declspec(dllexport) void setDraw()
    {
        chip.setDraw(false);
    }
}