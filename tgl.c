#include "headers/tgl.h"

#define WIDTH 10
#define HEIGHT 10

tgl__term_pixel_t PIXELS[WIDTH * HEIGHT];

int main() {
    tgl__clear_terminal();
    tgl__hide_cursor();
    tgl__move_cursor(0, 0);
    tgl__canvas_t canvas = tgl__make_canvas(PIXELS, WIDTH, HEIGHT);
    int i = 0;
    tgl__term_pixel_t pixel = {.value=' ', .foreground_color="\033[31m", .background_color="\033[41m"};
    while (i < 100) {
        tgl__fill_canvas(canvas, pixel);
        tgl__draw(canvas);
        i++;
    }

    tgl__show_cursor();
    return 0;
}
