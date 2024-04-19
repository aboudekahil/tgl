#include <math.h>

#define TGL_IMPLEMENTATION
#include "../headers/tgl.h"


#define WIDTH 200
#define HEIGHT 100
#define FIRE_SPEED 5

#define BACKGROUND_PIXEL (tgl__term_pixel_t) {.value=' ',.foreground_color={.value=0},.background_color={.value=0}}
#define RED_PIXEL (tgl__term_pixel_t) {.value=' ',.foreground_color={.value=0},.background_color={.value=0xFFFF0000}}
#define ORANGE_PIXEL (tgl__term_pixel_t) {.value=' ',.foreground_color={.value=0},.background_color={.value=0xFFFF3B00}}
#define ORANGE2_PIXEL (tgl__term_pixel_t) {.value=' ',.foreground_color={.value=0},.background_color={.value=0xFFFF5F00}}
#define YELLOW_PIXEL (tgl__term_pixel_t) {.value=' ',.foreground_color={.value=0},.background_color={.value=0xFFFFA600}}

static tgl__term_pixel_t pixels[WIDTH * HEIGHT];
static int64_t ys[WIDTH];
static int64_t ys2[WIDTH];

int main() {
    tgl__clear_terminal();
    tgl__hide_cursor();

    ys[0] = (rand() % (HEIGHT / 2));
    for (int i = 1; i < WIDTH; i++) {
        if(rand() % 2 == 0) {
            ys[i] = ys[i-1] + rand() % 2;
            ys[i] = fmin(ys[i], HEIGHT);
        } else {
            ys[i] = ys[i-1] - rand() % 2;
            ys[i] = fmax(ys[i], 0);
        }
    }

    for (int i = 0; i < WIDTH; i++) {
        ys2[i] = ys[i];
    }

    tgl__canvas_t canvas = tgl__make_canvas(pixels, WIDTH, HEIGHT, BACKGROUND_PIXEL);
    int i = 0;
    while (i++ < 1000000) {
        tgl__fill_canvas(canvas, BACKGROUND_PIXEL);

        for (int x = 0; x < WIDTH; x++) {
            for (int y = 0; y < ys[x]; y++) {
                tgl__draw_pixel(canvas, x, (HEIGHT - y)  , RED_PIXEL);
                tgl__draw_pixel(canvas, x, (HEIGHT - y) + (rand() % 5), ORANGE_PIXEL);
                tgl__draw_pixel(canvas, x, (HEIGHT - y) + (rand() % 7), ORANGE2_PIXEL);
                tgl__draw_pixel(canvas, x, (HEIGHT - y) + (rand() % 50), YELLOW_PIXEL);
            }
        }

        for (int j = 0; j < WIDTH; j++) {
            if(rand() % 2 == 0) {
                ys[j] = ys2[j] + (rand() % FIRE_SPEED);
                ys[j] = fmin(ys[j], HEIGHT);
            } else {
                ys[j] = ys2[j] - (rand() % FIRE_SPEED);
                ys[j] = fmax(ys[j], 0);
            }

            ys2[j] = ys2[j];
        }

        tgl__draw(canvas);
        usleep(1000000/60);
    }
    tgl__show_cursor();
    return EXIT_SUCCESS;
}