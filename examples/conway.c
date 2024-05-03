#define DEAD (tglTermPixel) {.value=' ', .foregroundColor={.value=0x00000000}, .backgroundColor={.value=0x00000000}}
#define BACKGROUND_PIXEL DEAD

#define ALIVE (tglTermPixel) {.value=' ', .foregroundColor={.value=0x00000000}, .backgroundColor={.value=0xFFFFFFFF}}

#define WIDTH 400
#define HEIGHT 100

// Use static for global storage duration across function calls
static bool grid[HEIGHT][WIDTH];
static bool new_grid[HEIGHT][WIDTH];

void setup(tglCanvas canvas) {
    // Initialize the grid with random data
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            grid[y][x] = rand() % 2;
        }
    }
}

int countNeighbors(int x, int y) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) {
                continue; // Skip the center cell
            }
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
                count += grid[ny][nx];
            }
        }
    }
    return count;
}

void update(float dt, bool *isRunning) {
    // Apply the rules of the Game of Life to each cell
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int neighbors = countNeighbors(x, y);
            if (grid[y][x]) { // Cell is alive
                new_grid[y][x] = (neighbors == 2 || neighbors == 3);
            } else { // Cell is dead
                new_grid[y][x] = (neighbors == 3);
            }
        }
    }
    // Copy the new state to the original grid
    memcpy(grid, new_grid, sizeof(grid));
}

void render(tglCanvas canvas) {
    tglFillCanvas(canvas, DEAD); // Clear the screen with dead cells

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (grid[y][x]) {
                tglDrawPixel(canvas, x, y, ALIVE);
            }
        }
    }
}
