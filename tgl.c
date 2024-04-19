#include <math.h>
#include <stdbool.h>

#define TGL_IMPLEMENTATION

#include "./headers/tgl.h"


#define WIDTH 200
#define HEIGHT 100
#define DEAD (tglTermPixel) {.value=' ',.foreground_color={.value=0},.background_color={.value=0}}
#define ALIVE (tgl__term_pixel_t) {.value=' ',.foreground_color={.value=0},.background_color={.value=0xFFFFFFFF}}

typedef bool(*rule_check_t)(bool **, size_t, size_t, size_t, size_t);

static tglTermPixel pixels[WIDTH * HEIGHT];


bool check_if_will_live(bool **grid, size_t width, size_t height, size_t i, size_t j, rule_check_t *rules);

void advance_grid(bool **grid, size_t width, size_t height);

bool c1(bool **, size_t width, size_t height, size_t i, size_t j);

static rule_check_t CHECKS[] = {c1};

int main() {
    tglClearTerminal();
    tglHideCursor();

    tglCanvas canvas = tglMakeCanvas(pixels, WIDTH, HEIGHT, DEAD);
    int i = 0;
    while (i++ < 1000000) {
        tglFillCanvas(canvas, DEAD);

        tglRender(canvas);
        usleep(1000000 / 60);
    }
    tgl__show_cursor();
    return EXIT_SUCCESS;
}

void advance_grid(bool **grid, size_t width, size_t height) {
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            grid[i][j] = check_if_will_live(grid, width, height, i, j, CHECKS);
        }
    }
}