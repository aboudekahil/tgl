#include <math.h>

#define TGL_IMPLEMENTATION

#include "./headers/tgl.h"


#define PI 3.14159265359f

#define WIDTH 200
#define HEIGHT 100
#define BACKGROUND_PIXEL (tgl__term_pixel_t) {.value=' ',.foreground_color={.value=0},.background_color={.value=0}}
#define GRID_COUNT 5
#define GRID_PAD (0.5f/GRID_COUNT)
#define GRID_SIZE ((GRID_COUNT - 1)*GRID_PAD)
#define CIRCLE_RADIUS 2
#define DT (1.f/60.f)

static tgl__term_pixel_t pixels[WIDTH * HEIGHT];
static float angle = 0;

int main() {
    tgl__clear_terminal();
    tgl__hide_cursor();

    tgl__canvas_t canvas = tgl__make_canvas(pixels, WIDTH, HEIGHT, BACKGROUND_PIXEL);
    int i = 0;
    float z_start;
    while (i++ < 1000000) {
        angle += 0.25f * PI * DT;
        z_start = cosf(0.5f * i * DT) + 1;
        tgl__fill_canvas(canvas, BACKGROUND_PIXEL);

        for (int ix = 0; ix < GRID_COUNT; ++ix) {
            for (int iy = 0; iy < GRID_COUNT; ++iy) {
                for (int iz = 0; iz < GRID_COUNT; ++iz) {
                    float x = ix * GRID_PAD - GRID_SIZE / 2;
                    float y = iy * GRID_PAD - GRID_SIZE / 2;
                    float z = z_start + iz * GRID_PAD;

                    float cx = 0.0f;
                    float cz = z_start + GRID_SIZE / 2;

                    float dx = x - cx;
                    float dz = z - cz;

                    float a = atan2f(dz, dx);
                    float m = sqrtf(dx * dx + dz * dz);

                    dx = cosf(a + angle) * m;
                    dz = sinf(a + angle) * m;

                    x = dx + cx;
                    z = dz + cz;

                    x /= z;
                    y /= z;

                    uint32_t r = ix * 255 / GRID_COUNT;
                    uint32_t g = iy * 255 / GRID_COUNT;
                    uint32_t b = iz * 255 / GRID_COUNT;
                    uint32_t color = 0xFF000000 | (r << (0 * 8)) | (g << (1 * 8)) | (b << (2 * 8));
                    tgl__fill_ellipse(canvas, (x + 1) / 2 * WIDTH, (y + 1) / 2 * HEIGHT, CIRCLE_RADIUS, CIRCLE_RADIUS,
                                      (tgl__term_pixel_t) {.value=' ',
                                              .foreground_color={.value=color},
                                              .background_color={.value=color}});
                }
            }
        }

        tgl__draw(canvas);
    }

    tgl__show_cursor();
    return EXIT_SUCCESS;
}