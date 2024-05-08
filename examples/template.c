#define BACKGROUND_PIXEL (tglTermPixel) {.value=' ', .foregroundColor={.value=0x00000000}, .backgroundColor={.value=0x00000000}}

#define WIDTH 400
#define HEIGHT 100

static int i = 0;

void setup(tglCanvas canvas) {
}

void render(tglCanvas canvas) {

}

void update(float dt, bool* isRunning) {
    if(i++ > 10) *isRunning = false;
}

