#include <math.h>

#define WIDTH 200
#define HEIGHT 100
#define FIRE_SPEED 5

#define BACKGROUND_PIXEL (tglTermPixel) {.value=' ',.foregroundColor={.value=0},.backgroundColor={.value=0}}
#define RED_PIXEL (tglTermPixel) {.value=' ',.foregroundColor={.value=0},.backgroundColor={.value=0xFFFF0000}}
#define ORANGE_PIXEL (tglTermPixel) {.value=' ',.foregroundColor={.value=0},.backgroundColor={.value=0xFFFF3B00}}
#define ORANGE2_PIXEL (tglTermPixel) {.value=' ',.foregroundColor={.value=0},.backgroundColor={.value=0xFFFF5F00}}
#define YELLOW_PIXEL (tglTermPixel) {.value=' ',.foregroundColor={.value=0},.backgroundColor={.value=0xFFFFA600}}

static int64_t ys[WIDTH];
static int64_t ys2[WIDTH];

void setup(tglCanvas canvas) {
    ys[0] = (rand() % (HEIGHT / 2));
    for (int i = 1; i < WIDTH; i++) {
        if (rand() % 2 == 0) {
            ys[i] = ys[i - 1] + rand() % 2;
            ys[i] = fmin(ys[i], HEIGHT);
        } else {
            ys[i] = ys[i - 1] - rand() % 2;
            ys[i] = fmax(ys[i], 0);
        }
    }

    for (int i = 0; i < WIDTH; i++) {
        ys2[i] = ys[i];
    }
}

void render(tglCanvas canvas) {
    tglFillCanvas(canvas, BACKGROUND_PIXEL);

    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < ys[x]; y++) {
            tglDrawPixel(canvas, x, (HEIGHT - y), RED_PIXEL);
            tglDrawPixel(canvas, x, (HEIGHT - y) + (rand() % 5), ORANGE_PIXEL);
            tglDrawPixel(canvas, x, (HEIGHT - y) + (rand() % 7), ORANGE2_PIXEL);
            tglDrawPixel(canvas, x, (HEIGHT - y) + (rand() % 50), YELLOW_PIXEL);
        }
    }
    tglRender(canvas);
}

void update(float dt, bool* isRunning) {



    for (int j = 0; j < WIDTH; j++) {
        if (rand() % 2 == 0) {
            ys[j] = ys2[j] + (rand() % FIRE_SPEED);
            ys[j] = fmin(ys[j], HEIGHT);
        } else {
            ys[j] = ys2[j] - (rand() % FIRE_SPEED);
            ys[j] = fmax(ys[j], 0);
        }

        ys2[j] = ys2[j];
    }
}