#include "headers/tgl.h"

#define WIDTH 40
#define HEIGHT 50

tgl__term_pixel_t PIXELS[WIDTH * HEIGHT];

tgl__term_pixel_t pixel = {.value=' ', .foreground_color="\033[31m", .background_color="\033[41m"};
tgl__term_pixel_t pixel2 = {.value=' ', .foreground_color="\033[39m", .background_color="\033[49m"};

int main() {
    tgl__clear_terminal();
    tgl__hide_cursor();
    tgl__move_cursor(0, 0);
    tgl__canvas_t canvas = tgl__make_canvas(PIXELS, WIDTH, HEIGHT, pixel2);
    int i = 0;
    while (i < 10000) {
        tgl__fill_canvas(canvas, pixel2);
        tgl__draw_ellipse(canvas, i % WIDTH, 20, 5, 10, pixel);
        tgl__draw(canvas);
        usleep(1000000 / 60);
        i++;
    }

    tgl__show_cursor();
    return 0;
}
