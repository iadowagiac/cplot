#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>

#define CI2(ci) ((unsigned char)(ci))
#define CI3(ci) ((unsigned char)(ci))
#define CI4(ci) ((unsigned char)(ci))
#define CI8(ci) ((unsigned char)(ci))

#define I1(i) ((unsigned char)(i))
#define I4(i) ((unsigned char)((i) * 15))
#define I8(i) ((unsigned char)((i) * 255))

#define IA4(i, a)  IA31(i, a)
#define IA8(i, a)  IA44(i, a)
#define IA16(i, a) IA88(i, a)

#define IA31(i, a) \
    (((unsigned char)((i) * 7) << 3) \
     ((unsigned char)(a)))

#define IA44(i, a) \
    (((unsigned char)((i) * 15) << 4) \
     ((unsigned char)((a) * 15)))

#define RGB24(r, g, b)     RGB888(r, g, b)

#define RGBA32(r, g, b, a) RGBA8888(r, g, b, a)

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)

#define IA88(i, a) \
    (((unsigned short)((i) * 255) << 8) | \
     ((unsigned short)((a) * 255)))

#define RGB565(r, g, b) \
    (((unsigned short)((r) * 31) << 11) | \
     ((unsigned short)((g) * 63) <<  5) | \
     ((unsigned short)((b) * 31)))

#define RGB888(r, g, b) \
    (((unsigned long)((r) * 255) << 16) | \
     ((unsigned long)((g) * 255) <<  8) | \
     ((unsigned long)((b) * 255)))

#define RGBA5551(r, g, b, a) \
    (((unsigned short)((r) * 31) << 11) | \
     ((unsigned short)((g) * 31) <<  6) | \
     ((unsigned short)((b) * 31) <<  1) | \
     ((unsigned short)(a)))

#define RGBA8888(r, g, b, a) \
    (((unsigned long)((r) * 255) << 24) | \
     ((unsigned long)((g) * 255) << 16) | \
     ((unsigned long)((b) * 255) <<  8) | \
     ((unsigned long)((a) * 255)))

#define ABGR8888(a, b, g, r) \
    (((unsigned long)((a) * 255) << 24) | \
     ((unsigned long)((b) * 255) << 16) | \
     ((unsigned long)((g) * 255) <<  8) | \
     ((unsigned long)((r) * 255)))

#else // LITTLE_ENDIAN

#define IA88(i, a) \
    (((unsigned short)((a) * 255) << 8) | \
     ((unsigned short)((i) * 255)))

#define RGB565(r, g, b) \
    (((unsigned short)((b) * 31) << 8) | \
     (((unsigned short)((g) * 63) & 0x7) << 13) | \
     ((unsigned short)((g) * 63) >> 3) | \
     ((unsigned short)((r) * 31) << 3))

#define RGB888(r, g, b) \
    (((unsigned long)((b) * 255) << 16) | \
     ((unsigned long)((g) * 255) <<  8) | \
     ((unsigned long)((r) * 255)))

#define RGBA5551(r, g, b, a) \
    (((unsigned short)(a) << 8) | \
     ((unsigned short)((b) * 31)) << 9) | \
     (((unsigned short)((g) * 31)) & 0x3) << 14) | \
     ((unsigned short)((g) * 31)) >> 2) | \
     ((unsigned short)((r) * 31)) << 3))

#define RGBA8888(r, g, b, a) \
    (((unsigned long)((a) * 255) << 24) | \
     ((unsigned long)((b) * 255) << 16) | \
     ((unsigned long)((g) * 255) <<  8) | \
     ((unsigned long)((r) * 255)))

#define ABGR8888(a, b, g, r) \
    (((unsigned long)((r) * 255) << 24) | \
     ((unsigned long)((g) * 255) << 16) | \
     ((unsigned long)((b) * 255) <<  8) | \
     ((unsigned long)((a) * 255)))

#endif // ndef LITTLE_ENDIAN

struct pixelmap
{
    unsigned short stride,
                   width,
                   height;
    unsigned short bpp;
    void *pixeldata;
};

bool createpixelmap(struct pixelmap *pixelmap, int width, int height,
                    int stride, int bpp, void *pixeldata);
void destroypixelmap(struct pixelmap *pixelmap);
unsigned long getpixel(struct pixelmap *pixelmap, int x, int y);
void setpixel(struct pixelmap *pixelmap, int x, int y, unsigned long value);
void setallpixels(struct pixelmap *pixelmap, unsigned long value);

struct graph
{
    float xmin, xmax,
          ymin, ymax;
    struct pixelmap pixelmap;
};

bool setupgraph(struct graph *graph, float xmin, float ymin, float xmax,
                float ymax, struct pixelmap *pixelmap);
void plotpoint(struct graph *graph, float _x, float _y, unsigned long value);

void setuprenderer(struct pixelmap *pixelmap);
void render(void);

#endif // ndef GRAPHICS_H
