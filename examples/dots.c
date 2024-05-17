#include <math.h>
#include "../headers/tgl.h"

#define TGL_IMPLEMENTATION

#define PI 3.14159265359f

#define WIDTH 200
#define HEIGHT 100
#define BACKGROUND_PIXEL (tglTermPixel) {.value=' ',.foregroundColor={.value=0xFF000000},.backgroundColor={.value=0xFF000000}}
#define GRID_COUNT 5
#define GRID_PAD (0.5f/GRID_COUNT)
#define GRID_SIZE ((GRID_COUNT - 1)*GRID_PAD)
#define CIRCLE_RADIUS 2

static float angle = 0;
static float z_start;
static float i = 0;

void setup(tglCanvas canvas) {};

void render(tglCanvas canvas) {
    tglFillCanvas(canvas, BACKGROUND_PIXEL);

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
                tglFillEllipse(canvas, (x + 1) / 2 * WIDTH, (y + 1) / 2 * HEIGHT, CIRCLE_RADIUS, CIRCLE_RADIUS,
                               (tglTermPixel) {.value=' ',
                                       .foregroundColor={.value=color},
                                       .backgroundColor={.value=color}});
            }
        }
    }

}

void update(float dt, bool *isRunning) {
    angle += 0.25f * PI * dt;
    z_start = cosf(0.9f * i) + 1;

    if (i > 1000) { (*isRunning) = false; };
    i += dt;
}
