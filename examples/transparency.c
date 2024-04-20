#define WIDTH 200
#define HEIGHT 100

#define BACKGROUND_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xFF000000}}
#define HALF_RED_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xAAFF0000}}
#define HALF_BLUE_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xAA0000FF}}
#define HALF_GREEN_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xAA00FF00}}
#define FULL_PURPLE_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xFFFF00FF}}

static int i = 0;
int rx = 0;

void setup(tglCanvas canvas) {}

void render(tglCanvas canvas) {
    tglFillCanvas(canvas, BACKGROUND_PIXEL);

    tglFillEllipse(canvas, rx % WIDTH, 35, 10, 5, HALF_RED_PIXEL);
    tglFillEllipse(canvas, 40, 25, 10, 5, HALF_BLUE_PIXEL);
    tglFillRect(canvas, 30, 30, 40, 20, HALF_GREEN_PIXEL);


    tglFillRect(canvas, 120, 30, 20, 10, FULL_PURPLE_PIXEL);
}

void update(float dt, bool *isRunning) {
    if (i++ > 1000) { *isRunning = false; }
    rx++;
}
