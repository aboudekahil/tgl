#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WIDTH 400
#define HEIGHT 100

#define BLACK (tglTermPixel) {.value=' ', .foregroundColor={.value=0x00000000}, .backgroundColor={.value=0x00000000}}
#define FIRE_COLORS_COUNT 37
tglTermPixel fireColors[FIRE_COLORS_COUNT];

static int fireBuffer[HEIGHT][WIDTH];

void generateFireColors() {
    for (int i = 0; i < FIRE_COLORS_COUNT; i++) {
        // Generate gradient from black to red to yellow to white
        uint32_t r = (i < 8) ? 0 : (i - 8) * 32;  // Ramp up red
        uint32_t g = (i < 16) ? 0 : (i - 16) * 16; // Ramp up green
        uint32_t b = 0;
        r = r > 255 ? 255 : r;
        g = g > 255 ? 255 : g;
        uint32_t colorValue = TGL_RGBA_TO_COLOR(r, g, b, 0xFFU);
        fireColors[i].value = ' ';
        fireColors[i].foregroundColor.value = 0x00000000;
        fireColors[i].backgroundColor.value = colorValue;
    }
}

void setup(tglCanvas canvas) {
    srand(time(NULL));
    generateFireColors();

    // Initialize the fire buffer
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            fireBuffer[y][x] = (y > HEIGHT - 4) ? FIRE_COLORS_COUNT - 1 : 0;
        }
    }
}

void updateFireIntensityPerPixel(int x, int y) {
    if (y == 0) {
        return; // Skip the top row
    }

    int decay = rand() % 3;
    int below = fireBuffer[y + 1][x];
    below = below - decay >= 0 ? below - decay : 0;
    int new_y = y - decay >= 0 ? y - decay : 0;
    fireBuffer[new_y][x] = below;
}

void update(float dt, bool *isRunning) {
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT - 1; y++) { // Skip the bottom row
            updateFireIntensityPerPixel(x, y);
        }
    }
}

void render(tglCanvas canvas) {
    tglFillCanvas(canvas, BLACK); // Clear the screen

    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            int intensity = fireBuffer[y][x];
            if (intensity > 0) {
                tglDrawPixel(canvas, x, y, fireColors[intensity]);
            }
        }
    }
}
