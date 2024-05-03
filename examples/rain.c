#include <stdlib.h>

#define WIDTH 400
#define HEIGHT 100
#define RAIN_DROP_COUNT 500
#define DROP_CHAR 'Y'

typedef struct {
    int x, y;
    int speed;
} RainDrop;

RainDrop rainDrops[RAIN_DROP_COUNT];

// Colors for rain
#define RAIN_PIXEL (tglTermPixel) {.value=DROP_CHAR, .foregroundColor={.value=0x99FF00FF}, .backgroundColor={.value=0x00000000}}
#define RAIN_PIXEL2 (tglTermPixel) {.value=DROP_CHAR, .foregroundColor={.value=0xFFFF00FF}, .backgroundColor={.value=0x00000000}}
#define RAIN_PIXEL3 (tglTermPixel) {.value=DROP_CHAR, .foregroundColor={.value=0x55FF00FF}, .backgroundColor={.value=0x00000000}}
#define RAIN_PIXEL1 (tglTermPixel) {.value=DROP_CHAR, .foregroundColor={.value=0x44FF00FF}, .backgroundColor={.value=0x00000000}}

void setup(tglCanvas canvas) {
    srand(time(NULL));

    // Initialize raindrops at random positions and with random speeds
    for (int i = 0; i < RAIN_DROP_COUNT; i++) {
        rainDrops[i].x = rand() % WIDTH;
        rainDrops[i].y = rand() % HEIGHT;
        rainDrops[i].speed = rand() % 4 + 1;  // Speeds between 1 and 4
    }
}

void update(float dt, bool* isRunning) {
    // Move each raindrop down according to its speed
    for (int i = 0; i < RAIN_DROP_COUNT; i++) {
        rainDrops[i].y += rainDrops[i].speed;
        // Reset drop to the top once it hits the bottom
        if (rainDrops[i].y >= HEIGHT) {
            rainDrops[i].y = 0;
            rainDrops[i].x = rand() % WIDTH;
            rainDrops[i].speed = rand() % 4 + 1;
        }
    }
}

void render(tglCanvas canvas) {
    tglFillCanvas(canvas, (tglTermPixel) {.value=' ', .foregroundColor={.value=0x00000000}, .backgroundColor={.value=0x00000000}}); // Clear screen

    // Draw each raindrop
    for (int i = 0; i < RAIN_DROP_COUNT; i++) {
        if (rainDrops[i].y < HEIGHT) {
            switch (rainDrops[i].speed) {
                case 1:
                    tglDrawPixel(canvas, rainDrops[i].x, rainDrops[i].y, RAIN_PIXEL);
                    break;
                case 2:
                    tglDrawPixel(canvas, rainDrops[i].x, rainDrops[i].y, RAIN_PIXEL2);
                    break;
                case 3:
                    tglDrawPixel(canvas, rainDrops[i].x, rainDrops[i].y, RAIN_PIXEL3);
                    break;
                case 4:
                    tglDrawPixel(canvas, rainDrops[i].x, rainDrops[i].y, RAIN_PIXEL1);
                    break;
            }
        }
    }
}
