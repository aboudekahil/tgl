#define WIDTH 200
#define HEIGHT 100

#define BACKGROUND_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xFF000000}}
#define HALF_RED_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xFEFF0000}}
#define HALF_BLUE_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xFE0000FF}}
#define HALF_GREEN_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xFE00FF00}}

static int i = 0;

void setup(tglCanvas canvas) {}

void render(tglCanvas canvas) {
    tglFillCanvas(canvas, BACKGROUND_PIXEL);

    tglDrawTriangle(canvas, i % 80, 5, 2, 40, 150, i % 80, HALF_RED_PIXEL);
    tglFillTriangle(canvas, WIDTH - (i % 80), 5, 2, 40, 150, i % 80, HALF_BLUE_PIXEL);
    tglFillTriangle(canvas, 80, 5, i % 100, i % 50, 150, 80, HALF_GREEN_PIXEL);
}

void update(float dt, bool *isRunning) {
    if (i++ > 1000) { *isRunning = false; }
}
