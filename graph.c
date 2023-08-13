/**
 * Graphs are built on top of pixelmaps, but use a different coordinate
 * system; one where (xmin, ymin) is the bottom left corner and
 * (xmax, ymax) is the top right corner.
 **/
#include "graphics.h"

bool setupgraph(struct graph *graph, float xmin, float ymin, float xmax,
                float ymax, struct pixelmap *pixelmap)
{
    graph->xmin = xmin;
    graph->xmax = xmax;
    graph->ymin = ymin;
    graph->ymax = ymax;
    graph->pixelmap = *pixelmap;
    return true;
}

void
plotpoint(struct graph *graph, float _x, float _y, unsigned long value)
{
    int x = (_x - graph->xmin)
          / (graph->xmax - graph->xmin) * graph->pixelmap.width,
        y = (graph->ymax - _y)
          / (graph->ymax - graph->ymin) * graph->pixelmap.height;
    if (x < 0 || y < 0 ||
        x >= graph->pixelmap.width || y >= graph->pixelmap.height)
        return;
    setpixel(&graph->pixelmap, x, y, value);
}
