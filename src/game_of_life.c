#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define false 0
#define true 1

typedef int boolean;

typedef struct {
    boolean alive;
} Cell;

typedef struct {
    int w;
    int h;
    Cell *cells;
} Grid;

/* New grid of dimensions width w and height h.
* Cells are uninitialized and undefined. */
Grid *Grid_new(int h, int w) {
    Grid *g = malloc(sizeof(Grid));
    if (!g) return NULL;
    g->w = w;
    g->h = h;
    g->cells = malloc(sizeof(Cell) * w * h);
    if (!g->cells) {
        free(g);
        return NULL;
    }
    return g;
}

/* Get the cell at (x, y) in grid g. */
Cell *Grid_get_cell(Grid *g, int x, int y) {
    x %= g->w;
    y %= g->h;
    if (x < 0)
        x = g->w - x;
    if (y < 0)
        y = g->h - y;
    return &(g->cells[y * g->w + x]);
}

/* Kill cell at (x, y) in grid g. */
void kill(Grid *g, int x, int y) {
    Grid_get_cell(g, x, y)->alive = false;
}

/* Give life to cell at (x, y) in grid g. */
void sustain(Grid *g, int x, int y) {
    Grid_get_cell(g, x, y)->alive = true;
}

/* Is cell at (x, y) in grid g alive? */

boolean alive(Grid *g, int x, int y) {
    return Grid_get_cell(g, x, y)->alive;
}

/* Kill all cells in grid g. */
void Grid_clear(Grid *g) {
    int x, y;
    for (x = 0; x < g->w; x++) {
        for (y = 0; y < g->h; y++)
            kill(g, x, y);
    }
}

/* Free grid g entirely. */
void Grid_free(Grid *g) {
    free(g->cells);
    free(g);
}

/* The number of neighbors for cell (x, y) in grid g */
int neighbor_count(Grid *g, int x, int y) {
    int count = 0;
    if (alive(g, x-1, y-1)) count++;
    if (alive(g, x-1, y)) count++;
    if (alive(g, x-1, y+1)) count++;
    if (alive(g, x, y-1)) count++;
    if (alive(g, x, y+1)) count++;
    if (alive(g, x+1, y-1)) count++;
    if (alive(g, x+1, y)) count++;
    if (alive(g, x+1, y+1)) count++;
    return count;
}

/* Will the cell at (x, y) in grid g survive (or come back to life)? */
boolean survives(Grid *g, int x, int y) {
    int n = neighbor_count(g, x, y);
    if (alive(g, x, y)) {
        if (n < 2)
            return false; /* starve */
        else if (n < 4)
            return true;
        else
            return false; /* over-population */
        } else {
            if (n == 3)
                return true; /* reproduction */
        }
    return false;
}

/* Fill next grid with new states of prev grid cells. */
void Grid_advance(Grid *g) {
    Grid *future = Grid_new(g->h, g->w);
    if (!future) {
        fprintf(stderr, "Allocation failure\n");
        return;
    }
    int x, y;
    for (y = 0; y < g->h; y++) {
        for (x = 0; x < g->w; x++) {
            if (survives(g, x, y))
                sustain(future, x, y);
            else
                kill(future, x, y);
        }
  }
    free(g->cells); /* free old cells */
    g->cells = future->cells; /* save new cells */
    free(future); /* make sure not to free future->cells */
}

void Grid_print(Grid *g) {
    int x, y;
    for (y = 0; y < g->h; y++) {
        for (x = 0; x < g->w; x++) {
            if (alive(g, x, y))
                printf("*");
            else
                printf(" ");
        }
        printf("\n");
  }
}

int main(void) {
    Grid *g;
    g = Grid_new(25, 80);
    Grid_clear(g);
    /* Glider */
    sustain(g, 5, 25);
    sustain(g, 6, 26);
    sustain(g, 4, 27);
    sustain(g, 5, 27);
    sustain(g, 6, 27);
    int i;
    for (i = 0; ; i++) {
        printf("Timer: %d\n", i);
        Grid_advance(g);
        Grid_print(g);
        sleep(1);
    }
  Grid_free(g);
    return 0;
}
