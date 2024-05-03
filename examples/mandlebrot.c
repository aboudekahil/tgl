#include <math.h>

#define WIDTH 400
#define HEIGHT 100

#define MAX_ITER 512
#define ESCAPE_RADIUS 10.0

tglTermPixel colors[MAX_ITER + 1];

static double centerX = -0.7;
static double centerY = -0.07;
static double scale = 2.5;
static double zoomFactor = 0.99;

void generateColors() {
    for (int i = 0; i < MAX_ITER; i++) {
        // Create a color gradient from blue to red
        double t = (double)i / MAX_ITER;
        uint32_t r = (uint32_t)(9 * (1 - t) * t * t * t * 255);
        uint32_t g = (uint32_t)(15 * (1 - t) * (1 - t) * t * t * 255);
        uint32_t b = (uint32_t)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
        uint32_t colorValue = TGL_RGBA_TO_COLOR(r, g, b, 0xFFU);
        colors[i].value = ' ';
        colors[i].foregroundColor.value = 0x00000000;
        colors[i].backgroundColor.value = colorValue;
    }
    // Set the escape color to black
    colors[MAX_ITER].value = ' ';
    colors[MAX_ITER].foregroundColor.value = 0x00000000;
    colors[MAX_ITER].backgroundColor.value = 0x00000000;
}

int mandelbrot(double cx, double cy) {
    double x = 0.0;
    double y = 0.0;
    int n = 0;
    while (x*x + y*y <= ESCAPE_RADIUS*ESCAPE_RADIUS && n < MAX_ITER) {
        double xtemp = x*x - y*y + cx;
        y = 2*x*y + cy;
        x = xtemp;
        n++;
    }
    return n;
}

void setup(tglCanvas canvas) {
    generateColors();
}

void update(float dt, bool* isRunning) {
    scale *= zoomFactor;
    centerX += scale * (zoomFactor - 1) * dt;
    centerY += scale * (zoomFactor - 1) * dt;
}

void render(tglCanvas canvas) {
    tglFillCanvas(canvas, colors[MAX_ITER]); // Clear with escape color (black)

    for (int px = 0; px < WIDTH; px++) {
        for (int py = 0; py < HEIGHT; py++) {
            double x0 = (px - WIDTH / 2) * scale / WIDTH + centerX;
            double y0 = (py - HEIGHT / 2) * scale / HEIGHT + centerY;
            int iteration = mandelbrot(x0, y0);
            tglDrawPixel(canvas, px, py, colors[iteration]);
        }
    }
}
