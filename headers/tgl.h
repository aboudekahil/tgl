// HEADER GUARD
// ------------------------------------------------------------------------
#ifndef TGL_LIB_H_
#define TGL_LIB_H_

// CPP EXTERN START
// --------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

// INTERNAL TGL MACROS
// -----------------------------------------------------------------
#ifndef TGLDEF
#define TGLDEF static inline
#endif

#ifndef ESC
#define ESC "\033"
#endif

#ifndef TGL_SET_PIXEL
#define TGL_SET_PIXEL(canvas, x, y, pixel)                                     \
  do {                                                                         \
    TGL_GET_PIXEL(canvas, x, y).value = pixel.value;                           \
    TGL_GET_PIXEL(canvas, x, y).backgroundColor.value =                        \
        tglMixColors(pixel.backgroundColor.value,                              \
                     TGL_GET_PIXEL(canvas, x, y).backgroundColor.value);       \
    TGL_GET_PIXEL(canvas, x, y).foregroundColor.value =                        \
        tglMixColors(pixel.foregroundColor.value,                              \
                     TGL_GET_PIXEL(canvas, x, y).foregroundColor.value);       \
  } while (0)
#endif

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#ifndef TGL_EXTRACT_RED
#define TGL_EXTRACT_RED(color) (((color) & 0x000000FF) >> (8 * 0))
#endif

#ifndef TGL_EXTRACT_GREEN
#define TGL_EXTRACT_GREEN(color) (((color) & 0x0000FF00) >> (8 * 1))
#endif

#ifndef TGL_EXTRACT_BLUE
#define TGL_EXTRACT_BLUE(color) (((color) & 0x00FF0000) >> (8 * 2))
#endif

#ifndef TGL_EXTRACT_ALPHA
#define TGL_EXTRACT_ALPHA(color) (((color) & 0xFF000000) >> (8 * 3))
#endif

#ifndef TGL_RGBA_TO_COLOR
#define TGL_RGBA_TO_COLOR(r, g, b, a)                                          \
  (r, g, b, a)((((r) & 0xFF) << (8 * 0)) | (((g) & 0xFF) << (8 * 1)) |         \
               (((b) & 0xFF) << (8 * 2)) | (((a) & 0xFF) << (8 * 3)))
#endif
#else

#ifndef TGL_EXTRACT_RED
#define TGL_EXTRACT_RED(color) (((color) & (uint32_t) 0x00FF0000) >> (uint32_t) (8 * 2))
#endif

#ifndef TGL_EXTRACT_GREEN
#define TGL_EXTRACT_GREEN(color) (((color) & (uint32_t) 0x0000FF00) >> (uint32_t) (8 * 1))
#endif

#ifndef TGL_EXTRACT_BLUE
#define TGL_EXTRACT_BLUE(color) (((color) & (uint32_t) 0x000000FF) >> (uint32_t) (8 * 0))
#endif

#ifndef TGL_EXTRACT_ALPHA
#define TGL_EXTRACT_ALPHA(color) (((color) & (uint32_t) 0xFF000000) >> (uint32_t) (8 * 3))
#define TGL_EXTRACT_ALPHA(color) (((color) & (uint32_t) 0xFF000000) >> (uint32_t) (8 * 3))
#endif

#ifndef TGL_RGBA_TO_COLOR
#define TGL_RGBA_TO_COLOR(r, g, b, a)                                          \
  ((((r) & (uint32_t) 0xFF) << (uint32_t) (8 * 2)) | (((g) & (uint32_t) 0xFF) << (uint32_t) (8 * 1)) |                     \
   (((b) & (uint32_t) 0xFF) << (uint32_t) (8 * 0)) | (((a) & (uint32_t) 0xFF) << (uint32_t) (8 * 3)))
#endif
#endif

#ifndef TGLAPI
#define TGLAPI TGLDEF __attribute_maybe_unused__
#endif

// Platform-specific includes and definitions
// ------------------------------------------
#if defined(__unix__) || defined(__unix) || defined(__linux) || \
    defined(__linux__) && !defined(OS_LINUX)

#define OS_LINUX

#elif defined(WIN32) || defined(_WIN32) || \
    defined(_WIN64) && !defined(OS_WINDOWS)

#define OS_WINDOWS

#else
#error Unsupported platform

#endif

// INCLUDES
// ----------------------------------------------------------------------------
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#if !defined(__cplusplus) && __STDC_VERSION__ <= 201710L
#include <stdbool.h>
#endif

#ifdef OS_LINUX

#include <sys/ioctl.h>
#include <unistd.h>

#endif

#ifdef OS_WINDOWS
#include <windows.h>
#endif

// EXTERNAL TGL MACROS
// -----------------------------------------------------------------
#define TGL_NULL_CANVAS ((tglCanvas){0})
#define TGL_GET_PIXEL(canvas, x, y) (canvas).pixels[(y) * (canvas).stride + (x)]
#define TGL_SWAP(T, a, b)                                                      \
  do {                                                                            \
    T t = a;                                                                   \
    a = b;                                                                     \
    b = t;                                                                     \
  } while(0)
#define TGL_SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define TGL_ABS(T, x) (TGL_SIGN(T, x) * (x))
#define TGL_IN_BOUNDS(canvas, x, y)                                            \
  x >= 0 && x < canvas.width &&y >= 0 && y < canvas.height

// DEFINITIONS
// -------------------------------------------------------------------------

typedef union {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    struct {
      uint8_t a;
      uint8_t r;
      uint8_t g;
      uint8_t b;
    };
#else
    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };
#endif
    uint32_t value;
} tglColor;

typedef struct {
    int64_t width;
    int64_t height;
} tglTermDim;

typedef struct {
    char value;
    tglColor foregroundColor;
    tglColor backgroundColor;
} tglTermPixel;

typedef struct {
    int64_t width;
    int64_t height;
    int64_t stride;
    tglTermPixel *pixels;
} tglCanvas;

// FUNCTION DEFINITIONS
// -------------------------------------------------------------------------

// Canvas function
// -------------------------------------------------------------------------

/**
 * @brief Returns a `tglCanvas`. This is the interface that has all the
 * information needed to render in the terminal.
 *
 * @param pixels A 1D array of `tglTermPixel` of length `width * height` ,
 * preferably not yet filled.
 * @param width Width of canvas.
 * @param height Height of canvas.
 * @param pixel Default pixel to fill canvas.
 * @return `tglCanvas` to be used to render.
 */
TGLAPI tglCanvas tglMakeCanvas(tglTermPixel *pixels, int64_t width,
                               int64_t height, tglTermPixel pixel);

/**
 * @brief Returns a `tglCanvas` that's a "subcanvas" of the canvas passed in the parameters (i.e a slice of the canvas).
 * Everything rendered in the subcanvas will be rendered in the main canvas. If the subcanvas doesn't fit inside the
 * canvas, it will return an empty `tglCanvas`.
 *
 * @param canvas The main canvas to take a "slice" of.
 * @param x The x position of the subcanvas inside the main canvas.
 * @param y The y position of the subcanvas inside the main canvas.
 * @param width The width of the subcanvas.
 * @param height The height of the subcanvas.
 * @return Returns the canvas slice.
 */
TGLAPI tglCanvas tglMakeSubcanvas(tglCanvas canvas, int64_t x, int64_t y,
                                  int64_t width, int64_t height);
// Outline drawing functions
// -------------------------------------------------------------------------

/**
 * @brief Draws a pixel on a canvas if within bounds.
 *
 * @param canvas The canvas to draw the pixel on.
 * @param x The x position of the pixel.
 * @param y The y position of the pixel.
 * @param pixel The pixel value to render the dot.
 */
TGLAPI void tglDrawPixel(tglCanvas canvas, int64_t x, int64_t y,
                         tglTermPixel pixel);

/**
 * @brief Draws a line on a canvas if within bounds.
 *
 * @param canvas The canvas to draw the line on.
 * @param x1 The x position of the first dot on the line.
 * @param y1 The y position of the first dot on the line.
 * @param x2 The x position of the second dot on the line.
 * @param y2 The y position of the second dot on the line.
 * @param pixel The pixel value to render the line.
 */
TGLAPI void tglDrawLine(tglCanvas canvas, int64_t x1, int64_t y1, int64_t x2,
                        int64_t y2, tglTermPixel pixel);

/**
 * @brief Draws a rectangle outline on a canvas if within bounds.
 *
 * @param canvas The canvas to draw the rectangle on.
 * @param x The x position of the top left dot of the rectangle.
 * @param y The y position of the top left dot of the rectangle.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param pixel The pixel value to render the rectangle.
 */
TGLAPI void tglDrawRect(tglCanvas canvas, int64_t x, int64_t y, int64_t w,
                        int64_t h, tglTermPixel pixel);

/**
 * @brief Draws an ellipse outline on a canvas if within bounds.
 *
 * @param canvas The canvas to draw the ellipse on.
 * @param x The x position of the center of the ellipse.
 * @param y The y position of the center of the ellipse.
 * @param r1 The horizontal radius of the ellipse.
 * @param r2 The vertical radius of the ellipse.
 * @param pixel The pixel value to render the ellipse.
 */
TGLAPI void tglDrawEllipse(tglCanvas canvas, int64_t x, int64_t y, int64_t r1,
                           int64_t r2, tglTermPixel pixel);

// Fill drawing functions
// -------------------------------------------------------------------------

/**
 * @brief Fills the canvas with the `pixel` value.
 *
 * @param canvas The canvas to fill.
 * @param pixel The pixel to fill the canvas.
 */
TGLAPI void tglFillCanvas(tglCanvas canvas, tglTermPixel pixel);

/**
 * @brief Draws a rectangle filled on a canvas if within bounds.
 *
 * @param canvas The canvas to draw the rectangle on.
 * @param x The x position of the top left dot of the rectangle.
 * @param y The y position of the top left dot of the rectangle.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param pixel The pixel value to render the rectangle.
 */
TGLAPI void tglFillRect(tglCanvas canvas, int64_t x, int64_t y, int64_t w,
                        int64_t h, tglTermPixel pixel);
/**
 * @brief Draws an ellipse filled on a canvas if within bounds.
 *
 * @param canvas The canvas to draw the ellipse on.
 * @param x The x position of the center of the ellipse.
 * @param y The y position of the center of the ellipse.
 * @param r1 The horizontal radius of the ellipse.
 * @param r2 The vertical radius of the ellipse.
 * @param pixel The pixel value to render the ellipse.
 */
TGLAPI void tglFillEllipse(tglCanvas canvas, int64_t x, int64_t y, int64_t r1,
                           int64_t r2, tglTermPixel pixel);

// Useful utility functions
// -------------------------------------------------------------------------

/**
 * @brief Mixes the given colors depending on the alpha values.
 *
 * @param color1 Color to mix with `color2`.
 * @param color2 Color to mix with `color1`.
 * @return Mixed color between `color1` and `color2`.
 */
TGLAPI uint32_t tglMixColors(uint32_t color1, uint32_t color2);

/**
 * @brief Returns whether or not the dot position is in bounds of given canvas.
 *
 * @param canvas Canvas to check bounds of.
 * @param x The x position to check if within bounds.
 * @param y The y position to check if within bounds.
 * @return True if within bounds false otherwise.
 */
TGLAPI bool tglIsInbounds(tglCanvas canvas, int64_t x, int64_t y);

/**
 * @brief Produce two ranges `x1...x2` and `y1...y2` that are safe to iterate over from rectangle.
 *
 * @param x x position of rectangle.
 * @param y y position of rectangle.
 * @param w Width of rectangle.
 * @param h Height of rectangle
 * @param canvasWidth Canvas width.
 * @param canvasHeight Canvas height.
 * @param x1 Safe x1.
 * @param x2 Safe x2.
 * @param y1 Safe y1.
 * @param y2 Safe y2.
 * @return True if rectangle is can be normalized, false otherwise.
 */
TGLAPI bool tglNormalizeRect(int64_t x, int64_t y, int64_t w, int64_t h,
                             int64_t canvasWidth, int64_t canvasHeight,
                             int64_t *x1, int64_t *x2, int64_t *y1,
                             int64_t *y2);

// Terminal information getters functions
// -------------------------------------------------------------------------

/**
 * @brief Returns true if the terminal supports rendering with colors.
 *
 * @return Returns true if the terminal supports rendering with colors.
 */
TGLAPI bool tglSupportsColor(void);

/**
 * @brief Getter to returns the width and height of the terminal.
 *
 * @return Struct containing width and height of terminal.
 */
TGLAPI tglTermDim tglGetTermSize(void);

// Terminal action functions
// -------------------------------------------------------------------------

/**
 * @brief Renders the content of a canvas on the terminal.
 */
TGLAPI void tglRender(tglCanvas canvas);

/**
 * @brief Clears the terminal.
 */
TGLAPI void tglClearTerminal(void);

/**
 * @brief Moves the cursor to the positions `x` and `y`.
 *
 * @param x x position on the terminal.
 * @param y y position on the terminal.
 */
TGLAPI void tglMoveCursor(int64_t x, int64_t y);

/**
 * @brief Makes the terminal cursor visible.
 */
TGLAPI void tglShowCursor(void);

/**
 * @brief Hides the cursor terminal.
 */
TGLAPI void tglHideCursor(void);

// CPP EXTERN END
// ----------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

// IMPLEMENTATION
// ----------------------------------------------------------------------
#ifdef TGL_IMPLEMENTATION

TGLAPI bool tglSupportsColor(void) {
#ifdef OS_LINUX
    const char *supportedTerms[] = {
            "ansi", "color", "console", "cygwin", "gnome", "konsole", "kterm",
            "linux", "msys", "putty", "rxvt", "screen", "vt100", "xterm"};

    const char *envP = getenv("TERM");
    if (envP == nullptr) {
        return false;
    }

    for (uint8_t i = 0;
         i < (uint8_t) (sizeof(supportedTerms) / sizeof(supportedTerms[0])); i++) {
        if (strstr(envP, supportedTerms[i]) != nullptr) {
            return true;
        }
    }

    return false;
#elif defined(OS_WINDOWS)
    static constexpr bool result = true;

    return result;
#endif
}

TGLAPI void tglClearTerminal(void) {
#ifdef OS_LINUX
    write(STDOUT_FILENO, ESC "[H\033[J", 7);
#else
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
      return;
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    if (!FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen,
                                    &cCharsWritten))
      return;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
      return;
    if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize,
                                    coordScreen, &cCharsWritten))
      return;

    SetConsoleCursorPosition(hConsole, coordScreen);
#endif
}

TGLAPI tglTermDim tglGetTermSize(void) {
    tglTermDim termSize;
#ifdef OS_LINUX
    struct winsize w = {};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    termSize.width = w.ws_col;
    termSize.height = w.ws_row;
#elif defined(OS_WINDOWS)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    termSize.width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    termSize.height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

#endif

    return termSize;
}

TGLAPI void tglHideCursor(void) {
#ifdef OS_LINUX
    printf(ESC "[?25l");
#elif defined(OS_WINDOWS)
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (consoleHandle == INVALID_HANDLE_VALUE) {
      return;
    }

    CONSOLE_CURSOR_INFO info = {0};
    info.dwSize = 100;
    info.bVisible = false;
    SetConsoleCursorInfo(consoleHandle, &info);
#endif
}

TGLAPI void tglShowCursor(void) {
#ifdef OS_LINUX
    printf(ESC "[?25h");
#elif defined(OS_WINDOWS)
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (consoleHandle == INVALID_HANDLE_VALUE) {
      return;
    }

    CONSOLE_CURSOR_INFO info = {0};
    info.dwSize = 100;
    info.bVisible = true;
    SetConsoleCursorInfo(consoleHandle, &info);
#endif
}

TGLAPI void tglMoveCursor(int64_t x, int64_t y) {
    printf(ESC "[%lu;%luH", x, y);
}

TGLAPI tglCanvas tglMakeCanvas(tglTermPixel *pixels, int64_t width,
                               int64_t height, tglTermPixel pixel) {
    tglCanvas canvas = {
            .pixels = pixels, .width = width, .height = height, .stride = width};

    tglFillCanvas(canvas, pixel);

    return canvas;
}

TGLAPI bool tglNormalizeRect(int64_t x, int64_t y, int64_t w, int64_t h,
                             int64_t canvasWidth, int64_t canvasHeight,
                             int64_t *x1, int64_t *x2, int64_t *y1,
                             int64_t *y2) {
    *x1 = x;
    *y1 = y;

    // Convert the rectangle to 2-points representation
    *x2 = *x1 + TGL_SIGN(int64_t, w) * (TGL_ABS(int64_t, w) - 1);
    if (*x1 > *x2) { TGL_SWAP(int64_t, *x1, *x2); }
    *y2 = *y1 + TGL_SIGN(int64_t, h) * (TGL_ABS(int64_t, h) - 1);
    if (*y1 > *y2) { TGL_SWAP(int64_t, *y1, *y2); }

    // Cull out invisible rectangle
    if (*x1 >= canvasWidth || *x2 < 0 || *y1 >= canvasHeight || *y2 < 0) {
        return false;
    }

    // Clamp the rectangle to the boundaries
    if (*x1 < 0) {
        *x1 = 0;
    }
    if (*x2 >= canvasWidth) {
        *x2 = canvasWidth - 1;
    }
    if (*y1 < 0) {
        *y1 = 0;
    }
    if (*y2 >= canvasHeight) {
        *y2 = canvasHeight - 1;
    }

    return true;
}

TGLAPI tglCanvas tglMakeSubcanvas(tglCanvas canvas, int64_t x, int64_t y,
                                  int64_t width, int64_t height) {
    int64_t x1;
    int64_t x2;
    int64_t y1;
    int64_t y2;
    if (!tglNormalizeRect(x, y, width, height, canvas.width, canvas.height, &x1,
                          &x2, &y1, &y2)) {
        return TGL_NULL_CANVAS;
    }

    canvas.pixels = &TGL_GET_PIXEL(canvas, x1, y1);
    canvas.width = x2 - x1 + 1;
    canvas.height = y2 - y1 + 1;
    return canvas;
}

TGLAPI void tglFillCanvas(tglCanvas canvas, tglTermPixel pixel) {
    for (int64_t y = 0; y < canvas.height; y++) {
        for (int64_t x = 0; x < canvas.width; x++) {
            TGL_SET_PIXEL(canvas, x, y, pixel);
        }
    }
}

TGLAPI void tglRender(tglCanvas canvas) {
    tglMoveCursor(0, 0);
    int64_t y = 0;
    for (; y < canvas.height - 1; y++) {
        for (int64_t x = 0; x < canvas.width; x++) {
            tglTermPixel pixel = TGL_GET_PIXEL(canvas, x, y);
            printf(ESC "[48;2;%d;%d;%dm", pixel.backgroundColor.r,
                   pixel.backgroundColor.g, pixel.backgroundColor.b);
            printf(ESC "[38;2;%d;%d;%dm", pixel.foregroundColor.r,
                   pixel.foregroundColor.g, pixel.foregroundColor.b);
            putchar(pixel.value);
        }
        fputs("\n", stdout);
    }

    for (int64_t x = 0; x < canvas.width; x++) {
        tglTermPixel pixel = TGL_GET_PIXEL(canvas, x, y);
        printf(ESC "[48;2;%d;%d;%dm", pixel.backgroundColor.r,
               pixel.backgroundColor.g, pixel.backgroundColor.b);
        printf(ESC "[38;2;%d;%d;%dm", pixel.foregroundColor.r,
               pixel.foregroundColor.g, pixel.foregroundColor.b);
        putchar(pixel.value);
    }
    fputs(ESC "[0m", stdout);
}

TGLAPI void tglFillRect(tglCanvas canvas, int64_t x, int64_t y, int64_t w,
                        int64_t h, tglTermPixel pixel) {
    int64_t x1;
    int64_t x2;
    int64_t y1;
    int64_t y2;

    if (!tglNormalizeRect(x, y, w, h, canvas.width, canvas.height, &x1, &x2, &y1,
                          &y2)) {
        return;
    }

    for (int64_t ix = x1; ix <= x2; ix++) {
        for (int64_t iy = y1; iy <= y2; iy++) {
            TGL_SET_PIXEL(canvas, ix, iy, pixel);
        }
    }
}

TGLAPI void tglDrawLine(tglCanvas canvas, int64_t x1, int64_t y1, int64_t x2,
                        int64_t y2, tglTermPixel pixel) {
    int64_t dx = x2 - x1;
    int64_t dy = y2 - y1;

    if (dx == 0 && dy == 0) {
        if (tglIsInbounds(canvas, x1, y1)) {
            TGL_SET_PIXEL(canvas, x1, y1, pixel);
        }
        return;
    }

    if (TGL_ABS(int64_t, dx) > TGL_ABS(int64_t, dy)) {
        if (x1 > x2) {
            TGL_SWAP(int64_t, x1, x2);
            TGL_SWAP(int64_t, y1, y2);
        }

        if (dx != 0) {
            for (int64_t x = x1; x <= x2; x++) {
                int64_t y = dy * (x - x1) / dx + y1;
                if (tglIsInbounds(canvas, x, y)) {
                    TGL_SET_PIXEL(canvas, x, y, pixel);
                }
            }
        }

    } else {
        if (y1 > y2) {
            TGL_SWAP(int64_t, x1, x2);
            TGL_SWAP(int64_t, y1, y2);
        }

        if (dy != 0) {
            for (int64_t y = y1; y <= y2; ++y) {
                int64_t x = dx * (y - y1) / dy + x1;
                if (tglIsInbounds(canvas, x, y)) {
                    TGL_SET_PIXEL(canvas, x, y, pixel);
                }
            }
        }
    }
}

TGLAPI bool tglIsInbounds(tglCanvas canvas, int64_t x, int64_t y) {
    return 0 <= x && x < canvas.width && 0 <= y &&
           y < canvas.height;
}

TGLAPI void tglDrawEllipse(tglCanvas canvas, int64_t x, int64_t y, int64_t r1,
                           int64_t r2, tglTermPixel pixel) {
    int64_t wx;
    int64_t wy;
    int64_t thresh;
    int64_t asq = r1 * r1; // TODO: OVERFLOW CHECK
    int64_t bsq = r2 * r2;
    int64_t xa;
    int64_t ya;
    int64_t ix = x;
    int64_t iy = y;
    int64_t ir1 = r1;
    int64_t ir2 = r2;

    TGL_SET_PIXEL(canvas, ix, y + r2, pixel);
    TGL_SET_PIXEL(canvas, ix, y - r2, pixel);

    wx = 0;
    wy = ir2;
    xa = 0;
    ya = asq * 2 * r2;
    thresh = asq / 4 - asq * r2;

    for (;;) {
        thresh += xa + bsq;

        if (thresh >= 0) {
            ya -= asq * 2;
            thresh -= ya;
            wy--;
        }

        xa += bsq * 2;
        wx++;

        if (xa >= ya) {
            break;
        }

        TGL_SET_PIXEL(canvas, ix + wx, iy - wy, pixel);
        TGL_SET_PIXEL(canvas, ix - wx, iy - wy, pixel);
        TGL_SET_PIXEL(canvas, ix + wx, iy + wy, pixel);
        TGL_SET_PIXEL(canvas, ix - wx, iy + wy, pixel);
    }
    TGL_SET_PIXEL(canvas, x + r1, iy, pixel);
    TGL_SET_PIXEL(canvas, x - r1, iy, pixel);

    wx = ir1;
    wy = 0;
    xa = (bsq) * 2 * r1;

    ya = 0;
    thresh = bsq / 4 - bsq * r1;

    for (;;) {
        thresh += ya + asq;

        if (thresh >= 0) {
            xa -= bsq * 2;
            thresh = thresh - xa;
            wx--;
        }

        ya += asq * 2;
        wy++;

        if (ya > xa) {
            break;
        }

        TGL_SET_PIXEL(canvas, ix + wx, iy - wy, pixel);
        TGL_SET_PIXEL(canvas, ix - wx, iy - wy, pixel);
        TGL_SET_PIXEL(canvas, ix + wx, iy + wy, pixel);
        TGL_SET_PIXEL(canvas, ix - wx, iy + wy, pixel);
    }
}

TGLAPI void tglDrawRect(tglCanvas canvas, int64_t x, int64_t y, int64_t w,
                        int64_t h, tglTermPixel pixel) {
    int64_t x1;
    int64_t x2;
    int64_t y1;
    int64_t y2;

    if (!tglNormalizeRect(x, y, w, h, canvas.width, canvas.height, &x1, &x2, &y1,
                          &y2)) {
        return;
    }

    for (int64_t i = x1; i <= x2; i++) {
        TGL_SET_PIXEL(canvas, i, y1, pixel);
    }

    for (int64_t i = x1; i <= x2; i++) {
        TGL_SET_PIXEL(canvas, i, y2, pixel);
    }

    for (int64_t i = y1; i <= y2; i++) {
        TGL_SET_PIXEL(canvas, x1, i, pixel);
    }

    for (int64_t i = y1; i <= y2; i++) {
        TGL_SET_PIXEL(canvas, x2, i, pixel);
    }
}

TGLAPI void tglFillEllipse(tglCanvas canvas, int64_t x, int64_t y, int64_t r1,
                           int64_t r2, tglTermPixel pixel) {
    int64_t x1 = x - r1;
    int64_t x2 = x + r1;
    int64_t y1 = y - r2;
    int64_t y2 = y + r2;

    if (x1 > canvas.width || y1 > canvas.height || x2 < 0 || y2 < 0) { return; }

    int64_t width = x2 - x1;
    int64_t height = y2 - y1;

    int64_t hh = height * height;
    int64_t ww = width * width;
    int64_t hhww = hh * ww;
    int64_t x0 = width;
    int64_t dx = 0;

    // do the horizontal diameter
    for (int64_t ix = -width; ix <= width; ix++) {
        if (TGL_IN_BOUNDS(canvas, x + ix, y)) {
            TGL_SET_PIXEL(canvas, x + ix, y, pixel);
        }
    }

    // now do both halves at the same time, away from the diameter
    for (int64_t iy = 1; iy <= height; iy++) {
        int64_t sx1 = x0 - (dx - 1); // try slopes of dx - 1 or more
        for (; sx1 > 0; sx1--) {
            if (sx1 * sx1 * hh + iy * iy * ww <= hhww) {
                break;
            }
        }
        dx = x0 - sx1; // current approximation of the slope
        x0 = sx1;

        for (int64_t ix = -x0; ix <= x0; ix++) {
            // TODO: REFACTOR TO GET x + ix not checked twice
            if (TGL_IN_BOUNDS(canvas, x + ix, y - iy)) {
                TGL_SET_PIXEL(canvas, x + ix, y - iy, pixel);
            }

            if (TGL_IN_BOUNDS(canvas, x + ix, y + iy)) {
                TGL_SET_PIXEL(canvas, x + ix, y + iy, pixel);
            }
        }
    }
}

TGLAPI void tglDrawPixel(tglCanvas canvas, int64_t x, int64_t y,
                         tglTermPixel pixel) {
    if (TGL_IN_BOUNDS(canvas, x, y)) {
        TGL_SET_PIXEL(canvas, x, y, pixel);
    }
}

TGLAPI uint32_t tglMixColors(uint32_t color1, uint32_t color2) {
    uint32_t r1 = TGL_EXTRACT_RED(color1);
    uint32_t g1 = TGL_EXTRACT_GREEN(color1);
    uint32_t b1 = TGL_EXTRACT_BLUE(color1);
    uint32_t a1 = TGL_EXTRACT_ALPHA(color1);

    if (a1 == 0xFF) {
        return color1;
    }

    uint32_t r2 = TGL_EXTRACT_RED(color2);
    uint32_t g2 = TGL_EXTRACT_GREEN(color2);
    uint32_t b2 = TGL_EXTRACT_BLUE(color2);
    uint32_t a2 = TGL_EXTRACT_ALPHA(color2);

    r1 = (r1 * a1 + r2 * a2) / 255;
    if (r1 > 255) {
        r1 = 255;
    }
    g1 = (g1 * a1 + g2 * a2) / 255;
    if (g1 > 255) {
        g1 = 255;
    }
    b1 = (b1 * a1 + b2 * a2) / 255;
    if (b1 > 255) {
        b1 = 255;
    }

    return TGL_RGBA_TO_COLOR(r1, g1, b1, a1);
}

#endif

// UNDEF INTERNAL MACROS
// ---------------------------------------------------------------
#undef OS_LINUX
#undef OS_WINDOWS
#undef TGLDEF
#undef ESC
#undef TGL_SET_PIXEL

#endif

// TODO: add 3d rendering
// TODO: add text rendering
