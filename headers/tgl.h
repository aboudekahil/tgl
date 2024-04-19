// HEADER GUARD ------------------------------------------------------------------------
#ifndef TGL_LIB_H_
#define TGL_LIB_H_

// CPP EXTERN START --------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

// INTERNAL TGL MACROS -----------------------------------------------------------------
#ifndef TGLDEF
#define TGLDEF static inline
#endif

#ifndef ESC
#define ESC "\033"
#endif

#ifndef TGL_MAYBE_UNUSED
#if defined(__GNUC__) || defined(__clang__)
#define TGL_MAYBE_UNUSED __attribute__((unused))
#else
#define TGL_MAYBE_UNUSED
#endif
#endif

// Platform-specific includes and definitions ------------------------------------------
#if defined(__unix__) || defined(__unix) || defined(__linux) || \
    defined(__linux__) && !defined(OS_LINUX)

#define OS_LINUX

#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64) && !defined(OS_WINDOWS)


#define OS_WINDOWS

#else
#error Unsupported platform

#endif

// INCLUDES ----------------------------------------------------------------------------
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

// EXTERNAL TGL MACROS -----------------------------------------------------------------
#define TGL_NULL_CANVAS ((tglCanvas) {0})
#define TGL_GET_PIXEL(canvas, x, y) (canvas).pixels[(y)*(canvas).stride + (x)]
#define TGL_SWAP(T, a, b) {T t = a; a = b; b = t;}
#define TGL_SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define TGL_ABS(T, x) (TGL_SIGN(T, x)*(x))
#define TGL_IN_BOUNDS(canvas, x, y) x >= 0 && x < canvas.width && y >= 0 && y < canvas.height



// DEFINITIONS -------------------------------------------------------------------------

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
    tglColor foreground_color;
    tglColor background_color;
} tglTermPixel;

typedef struct {
    int64_t width;
    int64_t height;
    int64_t stride;
    tglTermPixel *pixels;
} tglCanvas;

TGLDEF TGL_MAYBE_UNUSED tglCanvas
tglMakeCanvas(tglTermPixel *pixels, int64_t width, int64_t height, tglTermPixel pixel);


TGLDEF TGL_MAYBE_UNUSED void tglMoveCursor(int64_t x, int64_t y);

TGLDEF TGL_MAYBE_UNUSED bool tglSupportsColor(void);

TGLDEF TGL_MAYBE_UNUSED void tglClearTerminal(void);

TGLDEF TGL_MAYBE_UNUSED void tglHideCursor(void);

TGLDEF TGL_MAYBE_UNUSED void tgl__show_cursor(void);

TGLDEF TGL_MAYBE_UNUSED tglTermDim tglGetTermSize(void);


TGLDEF TGL_MAYBE_UNUSED bool tglNormalizeRect(int64_t x, int64_t y, int64_t w, int64_t h,
                                              int64_t canvas_width, int64_t canvas_height,
                                              int64_t *x1, int64_t *x2, int64_t *y1, int64_t *y2);

TGLDEF TGL_MAYBE_UNUSED tglCanvas
tglMakeSubcanvas(tglCanvas canvas, int64_t x, int64_t y, int64_t width, int64_t height);

TGLDEF TGL_MAYBE_UNUSED void tglFillCanvas(tglCanvas canvas, tglTermPixel pixel);

TGLDEF TGL_MAYBE_UNUSED void tglRender(tglCanvas canvas);

TGLDEF TGL_MAYBE_UNUSED void tglDrawPixel(tglCanvas canvas, int64_t x, int64_t y, tglTermPixel pixel);

TGLDEF TGL_MAYBE_UNUSED void
tglDrawLine(tglCanvas canvas, int64_t x1, int64_t y1, int64_t x2, int64_t y2, tglTermPixel pixel);

TGLDEF TGL_MAYBE_UNUSED bool tglIsInbounds(tglCanvas canvas, int64_t x, int64_t y);

TGLDEF TGL_MAYBE_UNUSED void
tglDrawRect(tglCanvas canvas, int64_t x, int64_t y, int64_t w, int64_t h, tglTermPixel pixel);

TGLDEF TGL_MAYBE_UNUSED void
tglDrawEllipse(tglCanvas canvas, int64_t x, int64_t y, int64_t r1, int64_t r2, tglTermPixel pixel);

TGLDEF TGL_MAYBE_UNUSED void
tglFillRect(tglCanvas canvas, int64_t x, int64_t y, int64_t w, int64_t h, tglTermPixel pixel);

TGLDEF TGL_MAYBE_UNUSED void
tglFillEllipse(tglCanvas canvas, int64_t x, int64_t y, int64_t r1, int64_t r2, tglTermPixel pixel);


// CPP EXTERN END ----------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

// IMPLEMENTATION ----------------------------------------------------------------------
#ifdef TGL_IMPLEMENTATION

TGLDEF TGL_MAYBE_UNUSED bool tglSupportsColor(void) {
#ifdef OS_LINUX
    const char *supported_terms[] = {
            "ansi", "color", "console", "cygwin", "gnome", "konsole", "kterm",
            "linux", "msys", "putty", "rxvt", "screen", "vt100", "xterm"};

    const char *env_p = getenv("TERM");
    if (env_p == nullptr) {
        return false;
    }

    for (uint8_t i = 0;
         i < (uint8_t) (sizeof(supported_terms) / sizeof(supported_terms[0]));
         i++) {
        if (strstr(env_p, supported_terms[i]) != nullptr) {
            return true;
        }
    }

    return false;
#elif defined(OS_WINDOWS)
    static constexpr bool result = true;

    return result;
#endif
}

TGLDEF TGL_MAYBE_UNUSED void tglClearTerminal(void) {
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

TGLDEF TGL_MAYBE_UNUSED tglTermDim tglGetTermSize(void) {
    tglTermDim term_size;
#ifdef OS_LINUX
    struct winsize w = {};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    term_size.width = w.ws_col;
    term_size.height = w.ws_row;
#elif defined(OS_WINDOWS)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    term_size.width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    term_size.height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

#endif

    return term_size;
}

TGLDEF TGL_MAYBE_UNUSED void tglHideCursor(void) {
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

TGLDEF TGL_MAYBE_UNUSED void tgl__show_cursor(void) {
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

TGLDEF TGL_MAYBE_UNUSED void tglMoveCursor(int64_t x, int64_t y) {
    printf(ESC "[%lu;%luH", x, y);
}

TGLDEF TGL_MAYBE_UNUSED tglCanvas
tglMakeCanvas(tglTermPixel *pixels, int64_t width, int64_t height, tglTermPixel pixel) {
    tglCanvas canvas = {
            .pixels=pixels,
            .width=width,
            .height=height,
            .stride=width
    };

    tglFillCanvas(canvas, pixel);

    return canvas;
}

TGLDEF TGL_MAYBE_UNUSED bool tglNormalizeRect(int64_t x, int64_t y, int64_t w, int64_t h,
                                              int64_t canvas_width, int64_t canvas_height,
                                              int64_t *x1, int64_t *x2, int64_t *y1, int64_t *y2) {
    *x1 = x;
    *y1 = y;

    // Convert the rectangle to 2-points representation
    *x2 = *x1 + TGL_SIGN(int64_t, w) * (TGL_ABS(int64_t, w) - 1);
    if (*x1 > *x2) TGL_SWAP(int64_t, *x1, *x2);
    *y2 = *y1 + TGL_SIGN(int64_t, h) * (TGL_ABS(int64_t, h) - 1);
    if (*y1 > *y2) TGL_SWAP(int64_t, *y1, *y2);

    // Cull out invisible rectangle
    if (*x1 >= canvas_width) return false;
    if (*x2 < 0) return false;
    if (*y1 >= canvas_height) return false;
    if (*y2 < 0) return false;

    // Clamp the rectangle to the boundaries
    if (*x1 < 0) *x1 = 0;
    if (*x2 >= canvas_width) *x2 = canvas_width - 1;
    if (*y1 < 0) *y1 = 0;
    if (*y2 >= canvas_height) *y2 = canvas_height - 1;

    return true;
}


TGLDEF TGL_MAYBE_UNUSED tglCanvas
tglMakeSubcanvas(tglCanvas canvas, int64_t x, int64_t y, int64_t width, int64_t height) {
    int64_t x1, x2, y1, y2;
    if (!tglNormalizeRect(x, y, width, height, canvas.width, canvas.height, &x1, &x2, &y1, &y2))
        return TGL_NULL_CANVAS;
    canvas.pixels = &TGL_GET_PIXEL(canvas, x1, y1);
    canvas.width = x2 - x1 + 1;
    canvas.height = y2 - y1 + 1;
    return canvas;
}

TGLDEF TGL_MAYBE_UNUSED void tglFillCanvas(tglCanvas canvas, tglTermPixel pixel) {
    for (int64_t y = 0; y < canvas.height; y++) {
        for (int64_t x = 0; x < canvas.width; x++) {
            TGL_GET_PIXEL(canvas, x, y) = pixel;
        }
    }
}

TGLDEF TGL_MAYBE_UNUSED void tglRender(tglCanvas canvas) {
    tglMoveCursor(0, 0);
    int64_t y = 0;
    for (; y < canvas.height - 1; y++) {
        for (int64_t x = 0; x < canvas.width; x++) {
            tglTermPixel pixel = TGL_GET_PIXEL(canvas, x, y);
            printf(ESC"[48;2;%d;%d;%dm", pixel.background_color.r, pixel.background_color.g, pixel.background_color.b);
            printf(ESC"[38;2;%d;%d;%dm", pixel.foreground_color.r, pixel.foreground_color.g, pixel.foreground_color.b);
            putchar(pixel.value);
        }
        fputs("\n", stdout);
    }

    for (int64_t x = 0; x < canvas.width; x++) {
        tglTermPixel pixel = TGL_GET_PIXEL(canvas, x, y);
        printf(ESC"[48;2;%d;%d;%dm", pixel.background_color.r, pixel.background_color.g, pixel.background_color.b);
        printf(ESC"[38;2;%d;%d;%dm", pixel.foreground_color.r, pixel.foreground_color.g, pixel.foreground_color.b);
        putchar(pixel.value);
    }
    fputs(ESC"[0m", stdout);
}

TGLDEF TGL_MAYBE_UNUSED void
tglFillRect(tglCanvas canvas, int64_t x, int64_t y, int64_t w, int64_t h, tglTermPixel pixel) {
    int64_t x1, x2, y1, y2;

    if (!tglNormalizeRect(x, y, w, h, canvas.width, canvas.height, &x1, &x2, &y1, &y2))
        return;

    for (int64_t ix = x1; ix <= x2; ix++) {
        for (int64_t iy = y1; iy <= y2; iy++) {
            TGL_GET_PIXEL(canvas, ix, iy) = pixel;
        }
    }
}

TGLDEF TGL_MAYBE_UNUSED void
tglDrawLine(tglCanvas canvas, int64_t x1, int64_t y1, int64_t x2, int64_t y2, tglTermPixel pixel) {
    int64_t dx = x2 - x1;
    int64_t dy = y2 - y1;

    if (dx == 0 && dy == 0) {
        if (tglIsInbounds(canvas, x1, y1)) {
            TGL_GET_PIXEL(canvas, x1, y1) = pixel;
        }
        return;
    }

    if (TGL_ABS(int64_t, dx) > TGL_ABS(int64_t, dy)) {
        if (x1 > x2) {
            TGL_SWAP(int64_t, x1, x2);
            TGL_SWAP(int64_t, y1, y2);
        }

        for (int64_t x = x1; x <= x2; x++) {
            int64_t y = dy * (x - x1) / dx + y1;
            if (tglIsInbounds(canvas, x, y)) {
                TGL_GET_PIXEL(canvas, x, y) = pixel;
            }
        }
    } else {
        if (y1 > y2) {
            TGL_SWAP(int64_t, x1, x2);
            TGL_SWAP(int64_t, y1, y2);
        }

        for (int64_t y = y1; y <= y2; ++y) {
            int64_t x = dx * (y - y1) / dy + x1;
            if (tglIsInbounds(canvas, x, y)) {
                TGL_GET_PIXEL(canvas, x, y) = pixel;
            }
        }
    }
}


TGLDEF TGL_MAYBE_UNUSED bool tglIsInbounds(tglCanvas canvas, int64_t x, int64_t y) {
    return 0 <= x && x < (int64_t) canvas.width && 0 <= y && y < (int64_t) canvas.height;
}

TGLDEF TGL_MAYBE_UNUSED void
tglDrawEllipse(tglCanvas canvas, int64_t x, int64_t y, int64_t r1, int64_t r2, tglTermPixel pixel) {
    int64_t wx, wy;
    int64_t thresh;
    int64_t asq = r1 * r1; // TODO: OVERFLOW CHECK
    int64_t bsq = r2 * r2;
    int64_t xa, ya;
    int64_t ix = x, iy = y;
    int64_t ir1 = r1, ir2 = r2;

    TGL_GET_PIXEL(canvas, ix, y + r2) = pixel;
    TGL_GET_PIXEL(canvas, ix, y - r2) = pixel;

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

        if (xa >= ya)
            break;


        TGL_GET_PIXEL(canvas, ix + wx, iy - wy) = pixel;
        TGL_GET_PIXEL(canvas, ix - wx, iy - wy) = pixel;
        TGL_GET_PIXEL(canvas, ix + wx, iy + wy) = pixel;
        TGL_GET_PIXEL(canvas, ix - wx, iy + wy) = pixel;
    }

    TGL_GET_PIXEL(canvas, (x + r1), iy) = pixel;
    TGL_GET_PIXEL(canvas, (x - r1), iy) = pixel;

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

        if (ya > xa)
            break;

        TGL_GET_PIXEL(canvas, ix + wx, iy - wy) = pixel;
        TGL_GET_PIXEL(canvas, ix - wx, iy - wy) = pixel;
        TGL_GET_PIXEL(canvas, ix + wx, iy + wy) = pixel;
        TGL_GET_PIXEL(canvas, ix - wx, iy + wy) = pixel;
    }
}

TGLDEF TGL_MAYBE_UNUSED void
tglDrawRect(tglCanvas canvas, int64_t x, int64_t y, int64_t w, int64_t h, tglTermPixel pixel) {
    int64_t x1, x2, y1, y2;

    if (!tglNormalizeRect(x, y, w, h, canvas.width, canvas.height, &x1, &x2, &y1, &y2))
        return;

    for (int64_t i = x1; i <= x2; i++) {
        TGL_GET_PIXEL(canvas, i, y1) = pixel;
    }

    for (int64_t i = x1; i <= x2; i++) {
        TGL_GET_PIXEL(canvas, i, y2) = pixel;
    }

    for (int64_t i = y1; i <= y2; i++) {
        TGL_GET_PIXEL(canvas, x1, i) = pixel;
    }

    for (int64_t i = y1; i <= y2; i++) {
        TGL_GET_PIXEL(canvas, x2, i) = pixel;
    }
}

TGLDEF TGL_MAYBE_UNUSED void
tglFillEllipse(tglCanvas canvas, int64_t x, int64_t y, int64_t r1, int64_t r2, tglTermPixel pixel) {
    int64_t x1 = x - r1, x2 = x + r1, y1 = y - r2, y2 = y + r2;

    if (x1 > canvas.width || y1 > canvas.height || x2 < 0 || y2 < 0) return;

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
            TGL_GET_PIXEL(canvas, x + ix, y) = pixel;
        }
    }

    // now do both halves at the same time, away from the diameter
    for (int64_t iy = 1; iy <= height; iy++) {
        int64_t sx1 = x0 - (dx - 1);  // try slopes of dx - 1 or more
        for (; sx1 > 0; sx1--) {
            if (sx1 * sx1 * hh + iy * iy * ww <= hhww) {
                break;
            }
        }
        dx = x0 - sx1;  // current approximation of the slope
        x0 = sx1;

        for (int64_t ix = -x0; ix <= x0; ix++) {
            // TODO: REFACTOR TO GET x + ix not checked twice
            if (TGL_IN_BOUNDS(canvas, x + ix, y - iy)) TGL_GET_PIXEL(canvas, x + ix, y - iy) = pixel;
            if (TGL_IN_BOUNDS(canvas, x + ix, y + iy)) TGL_GET_PIXEL(canvas, x + ix, y + iy) = pixel;
        }
    }
}

TGLDEF TGL_MAYBE_UNUSED void tglDrawPixel(tglCanvas canvas, int64_t x, int64_t y, tglTermPixel pixel) {
    if (TGL_IN_BOUNDS(canvas, x, y)) {
        TGL_GET_PIXEL(canvas, x, y) = pixel;
    }
}


#endif

// UNDEF INTERNAL MACROS ---------------------------------------------------------------
#undef OS_LINUX
#undef OS_WINDOWS
#undef TGLDEF
#undef ESC

#endif

// TODO: add 3d rendering
// TODO: add text rendering