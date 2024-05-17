#include <math.h>

#define WIDTH 200
#define HEIGHT 100
#define PI 3.14159265358979323846F

#define BACKGROUND_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xFF000000}}
#define WHITE_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xFFFFFFFF}}
#define RED_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xFFFF0000}}
#define GREEN_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xFF00FF00}}
#define BLUE_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xFF0000FF}}
#define PERIWINKLE_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xFFCCCCFF}}
#define YELLOW_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0xFF000000}, .backgroundColor={.value=0xFFFFFF00}}

typedef struct {
    float x, y, z;
} Vertex;


// Cube vertices
static Vertex vertices[8] = {
        {-1, -1, -1},
        {+1, -1, -1},
        {+1, +1, -1},
        {-1, +1, -1},
        {-1, -1, +1},
        {+1, -1, +1},
        {+1, +1, +1},
        {-1, +1, +1}
};


// Cube faces as two triangles each (6 faces, 2 triangles per face, 3 vertices per triangle)
int faces[12][3] = {
        {0, 1, 2},
        {0, 2, 3}, // Front face
        {1, 5, 6},
        {1, 6, 2}, // Right face
        {5, 4, 7},
        {5, 7, 6}, // Back face
        {4, 0, 3},
        {4, 3, 7}, // Left face
        {3, 2, 6},
        {3, 6, 7}, // Top face
        {1, 0, 4},
        {1, 4, 5}  // Bottom face
};


static int i = 0;
static float angle = 0;
static Vertex rotatedVertices[8];

void rotateX(Vertex *v, float angle) {
    float sinAngle = sinf(angle);
    float cosAngle = cosf(angle);
    float y = v->y * cosAngle - v->z * sinAngle;
    float z = v->y * sinAngle + v->z * cosAngle;
    v->y = y;
    v->z = z;
}

void rotateY(Vertex *v, float angle) {
    float sinAngle = sinf(angle);
    float cosAngle = cosf(angle);
    float x = v->x * cosAngle - v->z * sinAngle;
    float z = v->x * sinAngle + v->z * cosAngle;
    v->x = x;
    v->z = z;
}

void project(Vertex v, int64_t *x, int64_t *y) {
    float fov = 128;
    float viewer_distance = 8;
    *x = (int64_t) (v.x * fov / (v.z + viewer_distance)) + WIDTH / 2; // Adjusted for terminal center
    *y = (int64_t) (v.y * fov / (v.z + viewer_distance)) + HEIGHT / 2; // Adjusted for terminal center
}

void drawFilledTriangle(tglCanvas canvas, Vertex v1, Vertex v2, Vertex v3, tglTermPixel pixel) {
    int64_t x1;
    int64_t y1;
    int64_t x2;
    int64_t y2;
    int64_t x3;
    int64_t y3;
    project(v1, &x1, &y1);
    project(v2, &x2, &y2);
    project(v3, &x3, &y3);
    tglFillTriangle(canvas, x1, y1, x2, y2, x3, y3, pixel);

}

void drawCube(tglCanvas canvas, Vertex *vertices) {
    drawFilledTriangle(canvas, vertices[faces[0][0]], vertices[faces[0][1]], vertices[faces[0][2]], WHITE_PIXEL);
    drawFilledTriangle(canvas, vertices[faces[0 + 1][0]], vertices[faces[0 + 1][1]], vertices[faces[0 + 1][2]],
                       WHITE_PIXEL);

    drawFilledTriangle(canvas, vertices[faces[2][0]], vertices[faces[2][1]], vertices[faces[2][2]], RED_PIXEL);
    drawFilledTriangle(canvas, vertices[faces[2 + 1][0]], vertices[faces[2 + 1][1]], vertices[faces[2 + 1][2]],
                       RED_PIXEL);

    drawFilledTriangle(canvas, vertices[faces[4][0]], vertices[faces[4][1]], vertices[faces[4][2]], GREEN_PIXEL);
    drawFilledTriangle(canvas, vertices[faces[4 + 1][0]], vertices[faces[4 + 1][1]], vertices[faces[4 + 1][2]],
                       GREEN_PIXEL);

    drawFilledTriangle(canvas, vertices[faces[6][0]], vertices[faces[6][1]], vertices[faces[6][2]], BLUE_PIXEL);
    drawFilledTriangle(canvas, vertices[faces[6 + 1][0]], vertices[faces[6 + 1][1]], vertices[faces[6 + 1][2]],
                       BLUE_PIXEL);

    drawFilledTriangle(canvas, vertices[faces[8][0]], vertices[faces[8][1]], vertices[faces[8][2]], PERIWINKLE_PIXEL);
    drawFilledTriangle(canvas, vertices[faces[8 + 1][0]], vertices[faces[8 + 1][1]], vertices[faces[8 + 1][2]],
                       PERIWINKLE_PIXEL);

    drawFilledTriangle(canvas, vertices[faces[10][0]], vertices[faces[10][1]], vertices[faces[10][2]], YELLOW_PIXEL);
    drawFilledTriangle(canvas, vertices[faces[10 + 1][0]], vertices[faces[10 + 1][1]], vertices[faces[10 + 1][2]],
                       YELLOW_PIXEL);
}


void setup(tglCanvas canvas) {


}

void render(tglCanvas canvas) {
    tglFillCanvas(canvas, BACKGROUND_PIXEL);
    drawCube(canvas, rotatedVertices);
}

void update(float dt, bool *isRunning) {
    memcpy(rotatedVertices, vertices, sizeof(vertices));
    for (int8_t j = 0; j < 8; j++) {
        rotateX(&rotatedVertices[j], angle);
        rotateY(&rotatedVertices[j], angle);
    }

    if (i++ > 1000) { *isRunning = false; }
    angle += 0.05F;
    if (angle >= 2.F * PI) {
        angle -= 2.F * PI;
    }
}
