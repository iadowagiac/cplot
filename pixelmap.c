/**
 * In a pixelmap, (0, 0) is the coordinates of the top left corner
 * of the image. The stride is usually equal to the width of the
 * image, but in some cases, it's larger because the pixelmap is
 * for a subimage of some larger image.
 **/
#include <stdint.h>
#include <stdlib.h>

#include "graphics.h"

bool
createpixelmap(struct pixelmap *pixelmap, int width, int height, int stride,
               int bpp, void *pixeldata)
{
    if (stride == 0)
        stride = width;
    pixelmap->stride    = stride;
    pixelmap->width     = width;
    pixelmap->height    = height;
    pixelmap->bpp       = bpp;
    pixelmap->pixeldata = pixeldata;
    if (pixeldata == NULL)
        pixelmap->pixeldata = malloc(stride * height * bpp / 8);
    return pixelmap->pixeldata != NULL;
}

void
destroypixelmap(struct pixelmap *pixelmap)
{
    if (pixelmap->pixeldata != NULL)
    {
        free(pixelmap->pixeldata);
        pixelmap->pixeldata = NULL;
    }
}


unsigned long
getpixel(struct pixelmap *pixelmap, int x, int y)
{
    unsigned long i;
    switch (pixelmap->bpp)
    {
        case 1:
        case 4:
            break;
        case 8:
            i = y * pixelmap->stride + x;
            return ((uint8_t *)pixelmap->pixeldata)[i];
        case 16:
            i = y * pixelmap->stride + x;
            return ((uint16_t *)pixelmap->pixeldata)[i];
        case 24:
            i = 3 * (y * pixelmap->stride + x);
 #if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            return (unsigned long)((uint8_t *)pixelmap->pixeldata)
                       [i    ] << 16 |
                   (unsigned long)((uint8_t *)pixelmap->pixeldata)
                       [i + 1] <<  8 |
                   (unsigned long)((uint8_t *)pixelmap->pixeldata)
                       [i + 2]       ;
#else // LITTLE_ENDIAN
            return (unsigned long)((uint8_t *)pixelmap->pixeldata)
                       [i    ]       |
                   (unsigned long)((uint8_t *)pixelmap->pixeldata)
                       [i + 1] <<  8 |
                   (unsigned long)((uint8_t *)pixelmap->pixeldata)
                       [i + 2] << 16 ;
#endif // ndef LITTLE_ENDIAN
            break;
        case 32:
            i = y * pixelmap->stride + x;
            return ((uint32_t *)pixelmap->pixeldata)[i];
        default:
            return 0;
    }
}

void
setpixel(struct pixelmap *pixelmap, int x, int y, unsigned long value)
{
    unsigned long i;
    switch (pixelmap->bpp)
    {
        case 1:
        case 4:
            break;
        case 8:
            i = y * pixelmap->stride + x;
            ((uint8_t *)pixelmap->pixeldata)[i] = (uint8_t)value;
            break;
        case 16:
            i = y * pixelmap->stride + x;
            ((uint16_t *)pixelmap->pixeldata)[i] = (uint16_t)value;
            break;
        case 24:
            i = 3 * (y * pixelmap->stride + x);
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            ((uint8_t *)pixelmap->pixeldata)[i    ] =
                (uint8_t)(value >> 16 & 0xFF);
            ((uint8_t *)pixelmap->pixeldata)[i + 1] =
                (uint8_t)(value >>  8 & 0xFF);
            ((uint8_t *)pixelmap->pixeldata)[i + 2] =
                (uint8_t)(value       & 0xFF);
#else // LITTLE_ENDIAN
            ((uint8_t *)pixelmap->pixeldata)[i    ] =
                (uint8_t)(value       & 0xFF);
            ((uint8_t *)pixelmap->pixeldata)[i + 1] =
                (uint8_t)(value >>  8 & 0xFF);
            ((uint8_t *)pixelmap->pixeldata)[i + 2] =
                (uint8_t)(value >> 16 & 0xFF);
#endif // ndef LITTLE_ENDIAN
           break;
        case 32:
            i = y * pixelmap->stride + x;
            ((uint32_t *)pixelmap->pixeldata)[i] = (uint32_t)value;
            break;
    }
}

void
setallpixels(struct pixelmap *pixelmap, unsigned long value)
{
    unsigned long i;
    for (int y = 0; y < pixelmap->height; y++)
    {
        for (int x = 0; x < pixelmap->width; x++)
        {
            switch (pixelmap->bpp)
            {
                case 1:
                case 4:
                    break;
                case 8:
                    i = y * pixelmap->stride + x;
                    ((uint8_t *)pixelmap->pixeldata)[i] = (uint8_t)value;
                    break;
                case 16:
                    i = y * pixelmap->stride + x;
                    ((uint16_t *)pixelmap->pixeldata)[i] = (uint16_t)value;
                    break;
                case 24:
                    i = 3 * (y * pixelmap->stride + x);
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
                    ((uint8_t *)pixelmap->pixeldata)[i    ] =
                        (uint8_t)(value >> 16 & 0xFF);
                    ((uint8_t *)pixelmap->pixeldata)[i + 1] =
                        (uint8_t)(value >>  8 & 0xFF);
                    ((uint8_t *)pixelmap->pixeldata)[i + 2] =
                        (uint8_t)(value       & 0xFF);
#else // LITTLE_ENDIAN
                    ((uint8_t *)pixelmap->pixeldata)[i    ] =
                        (uint8_t)(value       & 0xFF);
                    ((uint8_t *)pixelmap->pixeldata)[i + 1] =
                        (uint8_t)(value >>  8 & 0xFF);
                    ((uint8_t *)pixelmap->pixeldata)[i + 2] =
                        (uint8_t)(value >> 16 & 0xFF);
#endif // ndef LITTLE_ENDIAN
                    break;
                case 32:
                    i = y * pixelmap->stride + x;
                    ((uint32_t *)pixelmap->pixeldata)[i] = (uint32_t)value;
                    break;
            }
        }
    }
}
