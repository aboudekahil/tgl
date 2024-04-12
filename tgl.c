#include "headers/tgl.h"
#include "signal.h"

tgl__term_pixel_t pixel = {.value=' ', .foreground_color={.value=0xFFFFFFFF}, .background_color={.value=0XFFCCCCFF}};
tgl__term_pixel_t pixel2 = {.value=' ', .foreground_color={.value=0}, .background_color={.value=0}};

void INTHANDLER(int);

#define WIDTH 100
#define HEIGHT 40

tgl__term_pixel_t PIXELS[WIDTH * HEIGHT];

int main() {

//    tgl__term_dim_t termDim = tgl__get_term_size();
//    PIXELS = malloc(termDim.height * termDim.width);

    signal(SIGINT, INTHANDLER);

    tgl__clear_terminal();
    tgl__hide_cursor();
    tgl__move_cursor(0, 0);
    tgl__canvas_t canvas = tgl__make_canvas(PIXELS, WIDTH, HEIGHT, pixel2);
    int i = 0;
    while (i < 1000) {
        tgl__fill_canvas(canvas, pixel2);
        tgl__fill_ellipse(canvas, (50 + 0) % WIDTH, 20, 10, 5, pixel);
        tgl__draw(canvas);
        usleep(1000000 / 120);
        i++;
    }

    tgl__show_cursor();
//    free(PIXELS);
    return 0;
}

void INTHANDLER(int) {
    tgl__show_cursor();
//    free(PIXELS);
    exit(0);
}
