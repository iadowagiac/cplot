#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>

#include <png.h>
#include <SDL2/SDL.h>

#include "graphics.h"

#define IMAGE_WIDTH  480
#define IMAGE_HEIGHT 272

#define WINDOW_WIDTH  480
#define WINDOW_HEIGHT 272

static struct timeval now, last_time;

static struct pixelmap pixelmap;
static struct graph graph;

void
fatal_error(char *error, ...)
{
    va_list args;
    va_start(args, error);
    fprintf(stderr, "error: ");
    vfprintf(stderr, error, args);
    fputc('\n', stderr);
    va_end(args);
    exit(EXIT_FAILURE);
}

float
getdt(void)
{
    struct timeval res;
    gettimeofday(&now, NULL);
    timersub(&now, &last_time, &res);
    return (float)res.tv_usec / 1000000;
}

// This is how you would print a graph if you were creating
// text based graphics instead of images.
void
printpixelmap(struct pixelmap *pixelmap)
{
    char *pixel = pixelmap->pixeldata;
    printf("\e[1;1H\e[2J");
    for (int y = 0; y < pixelmap->height; y++)
    {
        for (int x = 0; x < pixelmap->width; x++)
            putchar(pixel[y * pixelmap->stride + x]);
        printf("\r\n");
    }
}

// This is how you'd save a graph image to a PNG file.
void
saveimage(struct pixelmap *pixelmap, const char *filename)
{
    png_structp png_write_struct;
    png_infop png_info_struct;
    png_bytepp png_rows;
    int png_color_type;
    FILE *f = fopen(filename, "wb");
    if (f == NULL)
        return;
    png_write_struct =
        png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_write_struct == NULL)
    {
        fclose(f);
        return;
    }
    png_info_struct = png_create_info_struct(png_write_struct);
    if (png_info_struct == NULL)
    {
        png_destroy_write_struct(&png_write_struct, NULL);
        fclose(f);
        return;
    }
    if (setjmp(png_jmpbuf(png_write_struct)))
    {
        png_destroy_write_struct(&png_write_struct,
                                 &png_info_struct);
        fclose(f);
        return;
    }
    switch (pixelmap->bpp)
    {
        case 8:
            png_color_type = PNG_COLOR_TYPE_GRAY;
            break;
        case 24:
            png_color_type = PNG_COLOR_TYPE_RGB;
            break;
        case 32:
            png_color_type = PNG_COLOR_TYPE_RGB_ALPHA;
            break;
        default:
            fprintf(stderr,
                    "error: bpp must be 8, 24, or 32 to save a png.\n");
            png_destroy_write_struct(&png_write_struct,
                                     &png_info_struct);
            fclose(f);
            return;
    }
    png_set_IHDR(png_write_struct,
                 png_info_struct,
                 pixelmap->width,
                 pixelmap->height,
                 8,
                 png_color_type,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_rows = png_malloc(png_write_struct,
                          pixelmap->height * sizeof(png_byte *));
    for (int y = 0; y < pixelmap->height; y++)
    {
        switch (pixelmap->bpp)
        {
            case 8:
                png_rows[y] = (void *)&((uint8_t *)pixelmap->pixeldata)
                                           [y * pixelmap->stride];
                break;
            case 24:
                png_rows[y] = (void *)&((uint8_t *)pixelmap->pixeldata)
                                           [y * pixelmap->stride * 3];
                break;
            case 32:
                png_rows[y] = (void *)&((uint32_t *)pixelmap->pixeldata)
                                           [y * pixelmap->stride];
                break;
        }
    }
    png_init_io(png_write_struct, f);
    png_set_rows(png_write_struct, png_info_struct, png_rows);
    png_write_png(png_write_struct, png_info_struct,
                  PNG_TRANSFORM_IDENTITY, NULL);
    png_free(png_write_struct, png_rows);
    png_destroy_write_struct(&png_write_struct, &png_info_struct);
    fclose(f);
}

void
draw_scene(void)
{
    static float phase1_offset = 0.0,
                 phase2_offset = 0.0,
                 phase3_offset = 0.0;

    float dt = getdt();

    phase1_offset = fmod(phase1_offset + M_PI / 4 * dt, 2 * M_PI);
    phase2_offset = fmod(phase2_offset + M_PI / 8 * dt, 2 * M_PI);
    phase3_offset = fmod(phase3_offset + M_PI / 12 * dt, 2 * M_PI);

    // Clear the pixelmap.
    setallpixels(&pixelmap, ABGR8888(1.0, 0.9, 0.9, 0.9));

    // Plot a sine wave.
    for (float x = -2.0 * M_PI; x <= 2.0 * M_PI; x += 0.01)
    {
        plotpoint(&graph, x, 1.5 * sinf(x + 2.0 * M_PI / 360 * 000
                                          + phase1_offset),
                  ABGR8888(1.0, 0.0, 0.0, 1.0));
        plotpoint(&graph, x, 1.5 * sinf(x + 2.0 * M_PI / 360 * 120
                                          + phase2_offset),
                  ABGR8888(1.0, 0.0, 1.0, 0.0));
        plotpoint(&graph, x, 1.5 * sinf(x + 2.0 * M_PI / 360 * 240
                                          + phase3_offset),
                  ABGR8888(1.0, 1.0, 0.0, 0.0));
    }
}

/**
 * This is where my stuff ends and SDL begins.
 **/
void
update_screen(SDL_Renderer *renderer)
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rect =
    {
        .x = 0,
        .y = 0,
        .w = WINDOW_WIDTH,
        .h = WINDOW_HEIGHT,
    };

    draw_scene();

    surface = SDL_CreateRGBSurfaceFrom(pixelmap.pixeldata,
                                       pixelmap.width, pixelmap.height,
                                       32, 4 * pixelmap.stride,
                                       0xFF000000, 0x00FF0000,
                                       0x0000FF00, 0x000000FF);

    texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

int
main(int argc, char *argv[])
{

    bool running = true;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer;
    SDL_Event event;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT,
                                SDL_WINDOW_SHOWN,
                                &window, &renderer);
    if (window == NULL)
        fatal_error("failed to create window");
    SDL_SetWindowTitle(window, "cplot");

    // Create the pixelmap.
    createpixelmap(&pixelmap, IMAGE_WIDTH, IMAGE_HEIGHT, 0, 32, NULL);

    // Set up the graph.
    setupgraph(&graph, -2.0 * M_PI, -2.0, 2.0 * M_PI, 2.0, &pixelmap);

    // Print the pixelmap to stdout.
    //printpixelmap(&pixelmap);

    // Save a PNG file.
    //saveimage(&pixelmap, "test.png");

    gettimeofday(&last_time, NULL);

    while (running)
    {
        update_screen(renderer);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = 0;
                    break;
            }
        }
        last_time = now;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
