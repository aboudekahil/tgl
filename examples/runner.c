#define TGL_IMPLEMENTATION

#include <signal.h>
#include <time.h>
#include "../headers/tgl.h"

//void (*setup)(tglCanvas) = NULL;
void setup(tglCanvas canvas);
void render(tglCanvas canvas);
void update(float dt, bool* isRunning);

#include "./test.c"

#ifndef WIDTH
#define WIDTH 0
#endif

#ifndef HEIGHT
#define HEIGHT 0
#endif

#define TARGET_FPS 60
#define MS_PER_FRAME (1000 / TARGET_FPS)

#ifndef BACKGROUND_PIXEL
#define BACKGROUND_PIXEL (tglTermPixel) {.value=' ',.foregroundColor={.value=0xff000000},.backgroundColor={.value=0xff000000}}
#endif

uint64_t getCurrentTimeMillis() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned long long) (ts.tv_sec) * 1000 + (unsigned long long) (ts.tv_nsec) / 1000000;
}

void intHandler(int _) {
    tglShowCursor();
    exit(_);
}

tglTermPixel PIXELS[WIDTH * HEIGHT];

int main() {
    tglCanvas canvas = tglMakeCanvas(PIXELS, WIDTH, HEIGHT, BACKGROUND_PIXEL);

    tglClearTerminal();
    tglHideCursor();
    signal(SIGINT, intHandler);

    bool isRunning = true;

    uint64_t lastTime = getCurrentTimeMillis();
    uint64_t currentTime;
    float deltaTime;

    setup(canvas);

    while (isRunning) {
        currentTime = getCurrentTimeMillis();
        deltaTime = (float) (currentTime - lastTime) / 1000.0F;
        lastTime = currentTime;

        render(canvas);
        update(deltaTime, &isRunning);

        tglRender(canvas);
        uint64_t frameTime = getCurrentTimeMillis() - currentTime;
        if (frameTime < MS_PER_FRAME) {
            // Sleep to maintain consistent frame rate
            uint64_t sleepTime = MS_PER_FRAME - frameTime;
            struct timespec req = {0};
            req.tv_sec = sleepTime / 1000;
            req.tv_nsec = (sleepTime % 1000) * 1000000;
            nanosleep(&req, NULL);
        }
    }

    tglShowCursor();
    return EXIT_SUCCESS;
}
