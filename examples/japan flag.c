#define TGL_IMPLEMENTATION
#include "../headers/tgl.h"

#define BACKGROUND_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0x00000000}, .backgroundColor={.value=0x00000000}}
#define WHITE (tglTermPixel) {.value=' ', .foregroundColor={.value=0x00000000}, .backgroundColor={.value=0xFFFFFFFF}}
#define RED (tglTermPixel) {.value=' ', .foregroundColor={.value=0x00000000}, .backgroundColor={.value=0xFFFF0000}}

#define WIDTH 400
#define HEIGHT 100

static int i = 0;

void setup(tglCanvas canvas) {}

void render(tglCanvas canvas) {
    tglFillCanvas(canvas, BACKGROUND_PIXEL);

    tglFillRect(canvas, 0, 0, WIDTH, HEIGHT, WHITE);
    tglFillEllipse(canvas, WIDTH/2, HEIGHT/2, HEIGHT / 3, HEIGHT / 6, RED);

    tglRender(canvas);
}

void update(float dt, bool* isRunning) {
    if(i++ > 1000) *isRunning = false;
}

