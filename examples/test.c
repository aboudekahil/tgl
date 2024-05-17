#include <math.h>

#define BACKGROUND_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0x00000000}, .backgroundColor={.value=0x00000000}}
#define WIDTH 60
#define HEIGHT 40

static int i = 0;
#define PI 3.14159265359
static char luminanceChars[] = ".,-~:;=!*#$@";


void setup(tglCanvas canvas) {
    tglFillCanvas(canvas, BACKGROUND_PIXEL);
}

void render(tglCanvas canvas) {
    // Define the characters for shading based on luminance

    // Clear the canvas
    tglFillCanvas(canvas, BACKGROUND_PIXEL);

    // Torus parameters
    double A = i * 0.04;
    double B = i * 0.02;
    double cosA = cos(A), sinA = sin(A);
    double cosB = cos(B), sinB = sin(B);
    double R1 = 1, R2 = 2;
    double K2 = 5, K1 = 20;

    // Loop over theta and phi
    for (double theta = 0; theta < 2 * PI; theta += 0.07) {
        double costheta = cos(theta), sintheta = sin(theta);
        for (double phi = 0; phi < 2 * PI; phi += 0.02) {
            double cosphi = cos(phi), sinphi = sin(phi);
            double circlex = R2 + R1 * costheta;
            double circley = R1 * sintheta;

            // 3D coordinates after rotation
            double x = circlex * (cosB * cosphi + sinA * sinB * sinphi) - circley * cosA * sinB;
            double y = circlex * (sinB * cosphi - sinA * cosB * sinphi) + circley * cosA * cosB;
            double z = K2 + cosA * circlex * sinphi + circley * sinA;
            double ooz = 1 / z;
            int xp = (int) (WIDTH / 2 + K1 * ooz * x);
            int yp = (int) (HEIGHT / 2 - K1 * ooz * y);

            // Luminance
            double L = cosphi * costheta * sinB - cosA * costheta * sinphi - sinA * sintheta +
                       cosB * (cosA * sintheta - costheta * sinphi);
            if (L > 0) {
                int luminanceIndex = (int) (L * 8);
                char ch = luminanceChars[luminanceIndex];
                tglTermPixel pixel = {
                        .value = ch,
                        .foregroundColor = {.r=255, .g=255, .b=255, .a=255},
                        .backgroundColor = {.r=0, .g=0, .b=0, .a=255}
                };
                tglDrawPixel(canvas, xp, yp, pixel);
            }
        }
    }
}

void update(float dt, bool *isRunning) {
    if (i++ > 10) {
        *isRunning = false;
    }
}
