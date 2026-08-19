#include "snkspr.h"
#include <stdint.h>
#include <fileioc.h>

#define SNKSPR_HEADER_SIZE 0

unsigned char *SNKSPR_appvar[18] =
{
    (unsigned char*)0,
    (unsigned char*)220,
    (unsigned char*)372,
    (unsigned char*)533,
    (unsigned char*)700,
    (unsigned char*)910,
    (unsigned char*)1160,
    (unsigned char*)1395,
    (unsigned char*)1468,
    (unsigned char*)1669,
    (unsigned char*)1711,
    (unsigned char*)1781,
    (unsigned char*)1834,
    (unsigned char*)1888,
    (unsigned char*)1930,
    (unsigned char*)1956,
    (unsigned char*)1980,
    (unsigned char*)2003,
};

unsigned char SNKSPR_init(void)
{
    uintptr_t data;
    unsigned int i;
    uint8_t appvar;

    appvar = ti_Open("SNKSPR", "r");
    if (appvar == 0)
    {
        return 0;
    }

    data = (uintptr_t)ti_GetDataPtr(appvar) - (uintptr_t)SNKSPR_appvar[0] + SNKSPR_HEADER_SIZE;
    for (i = 0; i < 18; i++)
    {
        SNKSPR_appvar[i] += data;
    }

    ti_Close(appvar);

    return 1;
}

