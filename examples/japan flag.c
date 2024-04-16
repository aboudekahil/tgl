#define TGL_IMPLEMENTATION
#include "../headers/tgl.h"

#define WIDTH 400
#define HEIGHT 100

static tgl__term_pixel_t pixels[WIDTH * HEIGHT];

int main() {
    tgl__clear_terminal();
    tgl__hide_cursor();
    tgl__term_pixel_t empty_pixel = {.value=' ', .foreground_color={.value=0x00000000}, .background_color={.value=0x00000000}};
    tgl__term_pixel_t white = {.value=' ', .foreground_color={.value=0x00000000}, .background_color={.value=0xFFFFFFFF}};
    tgl__term_pixel_t red = {.value=' ', .foreground_color={.value=0x00000000}, .background_color={.value=0xFFFF0000}};
    tgl__canvas_t canvas = tgl__make_canvas(pixels, WIDTH, HEIGHT, empty_pixel);

    int i = 0;
    while (i++ < 100000) {
        tgl__fill_canvas(canvas, empty_pixel);

        tgl__fill_rect(canvas, 0, 0, WIDTH, HEIGHT, white);
        tgl__fill_ellipse(canvas, WIDTH/2, HEIGHT/2, HEIGHT / 3, HEIGHT / 6, red);

        tgl__draw(canvas);

        usleep(100000 / 60);
    }

    tgl__show_cursor();
    return EXIT_SUCCESS;
}