#include <stdlib.h>
#include <math.h>

#define BACKGROUND_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0x00000000}, .backgroundColor={.value=0x00000000}}
#define WHITE (tglTermPixel) {.value=' ', .foregroundColor={.value=0x00000000}, .backgroundColor={.value=0xFFFFFFFF}}
#define RED (tglTermPixel) {.value=' ', .foregroundColor={.value=0x00000000}, .backgroundColor={.value=0xFFFF0000}}

#define WIDTH 400
#define HEIGHT 100
#define PARTICLE_COUNT_X 20
#define PARTICLE_COUNT_Y 10
#define PARTICLE_SPACING 20
#define STIFFNESS 0.28
#define DAMPING 0.1
#define GRAVITY 0.1
#define TIME_STEP 0.5
#define WIND 0.05

typedef struct {
    float x, y;
    float vx, vy;  // velocities
    bool fixed;
} Particle;

Particle particles[PARTICLE_COUNT_X * PARTICLE_COUNT_Y];

void setupParticles() {
    for (int y = 0; y < PARTICLE_COUNT_Y; ++y) {
        for (int x = 0; x < PARTICLE_COUNT_X; ++x) {
            int index = y * PARTICLE_COUNT_X + x;
            particles[index].x = x * PARTICLE_SPACING;
            particles[index].y = y * PARTICLE_SPACING;
            particles[index].vx = 0;
            particles[index].vy = 0;
            particles[index].fixed = (y == 0); // fix the top row of particles
        }
    }
}

void updateParticlePhysics(float dt) {
    for (int i = 0; i < PARTICLE_COUNT_X * PARTICLE_COUNT_Y; ++i) {
        if (!particles[i].fixed) {
            particles[i].vx += WIND;
            particles[i].vy += GRAVITY * dt;
            float newX = particles[i].x + particles[i].vx * dt;
            float newY = particles[i].y + particles[i].vy * dt;
            particles[i].vx *= (1 - DAMPING);
            particles[i].vy *= (1 - DAMPING);
            particles[i].x = newX;
            particles[i].y = newY;
        }
    }
}

void applyConstraints() {
    for (int y = 0; y < PARTICLE_COUNT_Y; ++y) {
        for (int x = 0; x < PARTICLE_COUNT_X; ++x) {
            int index = y * PARTICLE_COUNT_X + x;
            if (x > 0) { // Horizontal spring
                int left = index - 1;
                float dx = particles[index].x - particles[left].x;
                float dy = particles[index].y - particles[left].y;
                float distance = sqrt(dx * dx + dy * dy);
                float diff = (distance - PARTICLE_SPACING) / distance;
                if (!particles[left].fixed) {
                    particles[left].x += dx * STIFFNESS * diff;
                    particles[left].y += dy * STIFFNESS * diff;
                }
                if (!particles[index].fixed) {
                    particles[index].x -= dx * STIFFNESS * diff;
                    particles[index].y -= dy * STIFFNESS * diff;
                }
            }
            if (y > 0) { // Vertical spring
                int above = index - PARTICLE_COUNT_X;
                float dx = particles[index].x - particles[above].x;
                float dy = particles[index].y - particles[above].y;
                float distance = sqrt(dx * dx + dy * dy);
                float diff = (distance - PARTICLE_SPACING) / distance;
                if (!particles[above].fixed) {
                    particles[above].x += dx * STIFFNESS * diff;
                    particles[above].y += dy * STIFFNESS * diff;
                }
                if (!particles[index].fixed) {
                    particles[index].x -= dx * STIFFNESS * diff;
                    particles[index].y -= dy * STIFFNESS * diff;
                }
            }
        }
    }
}

void setup(tglCanvas canvas) {
    setupParticles();
}

void render(tglCanvas canvas) {
    tglFillCanvas(canvas, BACKGROUND_PIXEL);
    for (int i = 0; i < PARTICLE_COUNT_X * PARTICLE_COUNT_Y; ++i) {
        tglDrawPixel(canvas, (int) particles[i].x, (int) particles[i].y, RED);
    }
}

void update(float dt, bool *isRunning) {
    updateParticlePhysics(dt);
    applyConstraints();
    static int frameCount = 0;
    if (++frameCount > 200) *isRunning = false;
}
