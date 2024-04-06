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

#define ESC "\033"
#if defined(__GNUC__) || defined(__clang__)
#define TGL_MAYBE_UNUSED __attribute__((unused))
#else
#define TGL_MAYBE_UNUSED
#endif

// Platform-specific includes and definitions ------------------------------------------
#if defined(__unix__) || defined(__unix) || defined(__linux) || \
    defined(__linux__)

#include <sys/ioctl.h>
#include <unistd.h>

#define OS_LINUX

#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64)

#include <windows.h>

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

// EXTERNAL TGL MACROS -----------------------------------------------------------------
#define TGL__NULL_CANVAS ((tgl__canvas_t) {0})
#define TGL__GET_PIXEL(canvas, x, y) (canvas).pixels[(y)*(canvas).stride + (x)]
#define TGL__SWAP(T, a, b) {T t = a; a = b; b = t;}
#define TGL__SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define TGL__ABS(T, x) (TGL__SIGN(T, x)*(x))


// DEFINITIONS -------------------------------------------------------------------------

typedef struct tgl__term_dim {
    int64_t width;
    int64_t height;
} tgl__term_dim_t;

typedef struct tgl__term_pixel {
    char value;
    char *foreground_color;
    char *background_color;
} tgl__term_pixel_t;

typedef struct tgl__canvas {
    int64_t width;
    int64_t height;
    int64_t stride;
    tgl__term_pixel_t *pixels;
} tgl__canvas_t;

TGLDEF TGL_MAYBE_UNUSED tgl__canvas_t
tgl__make_canvas(tgl__term_pixel_t *pixels, int64_t width, int64_t height, tgl__term_pixel_t pixel);

TGLDEF TGL_MAYBE_UNUSED void tgl__move_cursor(int64_t x, int64_t y);

TGLDEF TGL_MAYBE_UNUSED bool tgl__supports_color(void);

TGLDEF TGL_MAYBE_UNUSED void tgl__clear_terminal(void);

TGLDEF TGL_MAYBE_UNUSED void tgl__hide_cursor(void);

TGLDEF TGL_MAYBE_UNUSED void tgl__show_cursor(void);

TGLDEF TGL_MAYBE_UNUSED tgl__term_dim_t tgl__get_term_size(void);


TGLDEF TGL_MAYBE_UNUSED bool tgl__normalize_rect(int64_t x, int64_t y, int64_t w, int64_t h,
                                                 int64_t canvas_width, int64_t canvas_height,
                                                 int64_t *x1, int64_t *x2, int64_t *y1, int64_t *y2);

TGLDEF TGL_MAYBE_UNUSED tgl__canvas_t
tgl__make_subcanvas(tgl__canvas_t canvas, int64_t x, int64_t y, int64_t width, int64_t height);

TGLDEF TGL_MAYBE_UNUSED void tgl__fill_canvas(tgl__canvas_t canvas, tgl__term_pixel_t pixel);

TGLDEF TGL_MAYBE_UNUSED void tgl__draw(tgl__canvas_t canvas);

TGLDEF TGL_MAYBE_UNUSED void
tgl__draw_rect(tgl__canvas_t canvas, int64_t x, int64_t y, int64_t w, int64_t h, tgl__term_pixel_t pixel);

TGLDEF TGL_MAYBE_UNUSED void
tgl__draw_line(tgl__canvas_t canvas, int64_t x1, int64_t y1, int64_t x2, int64_t y2, tgl__term_pixel_t pixel);

TGLDEF TGL_MAYBE_UNUSED bool tgl__is_inbounds(tgl__canvas_t canvas, int64_t x, int64_t y);

TGLDEF TGL_MAYBE_UNUSED void
tgl__draw_ellipse(tgl__canvas_t canvas, int64_t x, int64_t y, int64_t r1, int64_t r2, tgl__term_pixel_t pixel);

// CPP EXTERN END ----------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

// IMPLEMENTATION ----------------------------------------------------------------------
#ifdef TGL_IMPLEMENTATION

TGLDEF TGL_MAYBE_UNUSED bool tgl__supports_color(void) {
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

TGLDEF TGL_MAYBE_UNUSED void tgl__clear_terminal(void) {
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

TGLDEF TGL_MAYBE_UNUSED tgl__term_dim_t tgl__get_term_size(void) {
    tgl__term_dim_t term_size;
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

TGLDEF TGL_MAYBE_UNUSED void tgl__hide_cursor(void) {
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

TGLDEF TGL_MAYBE_UNUSED void tgl__move_cursor(int64_t x, int64_t y) {
    printf(ESC "[%lu;%luH", x, y);
}

TGLDEF TGL_MAYBE_UNUSED tgl__canvas_t
tgl__make_canvas(tgl__term_pixel_t *pixels, int64_t width, int64_t height, tgl__term_pixel_t pixel) {
    tgl__canvas_t canvas = {
            .pixels=pixels,
            .width=width,
            .height=height,
            .stride=width
    };

    tgl__fill_canvas(canvas, pixel);

    return canvas;
}

TGLDEF TGL_MAYBE_UNUSED bool tgl__normalize_rect(int64_t x, int64_t y, int64_t w, int64_t h,
                                                 int64_t canvas_width, int64_t canvas_height,
                                                 int64_t *x1, int64_t *x2, int64_t *y1, int64_t *y2) {
    *x1 = x;
    *y1 = y;

    // Convert the rectangle to 2-points representation
    *x2 = *x1 + TGL__SIGN(int64_t, w) * (TGL__ABS(int64_t, w) - 1);
    if (*x1 > *x2) TGL__SWAP(int64_t, *x1, *x2);
    *y2 = *y1 + TGL__SIGN(int64_t, h) * (TGL__ABS(int64_t, h) - 1);
    if (*y1 > *y2) TGL__SWAP(int64_t, *y1, *y2);

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


TGLDEF TGL_MAYBE_UNUSED tgl__canvas_t
tgl__make_subcanvas(tgl__canvas_t canvas, int64_t x, int64_t y, int64_t width, int64_t height) {
    int64_t x1, x2, y1, y2;
    if (!tgl__normalize_rect(x, y, width, height, canvas.width, canvas.height, &x1, &x2, &y1, &y2))
        return TGL__NULL_CANVAS;
    canvas.pixels = &TGL__GET_PIXEL(canvas, x1, y1);
    canvas.width = x2 - x1 + 1;
    canvas.height = y2 - y1 + 1;
    return canvas;
}

TGLDEF TGL_MAYBE_UNUSED void tgl__fill_canvas(tgl__canvas_t canvas, tgl__term_pixel_t pixel) {
    for (int64_t y = 0; y < canvas.height; y++) {
        for (int64_t x = 0; x < canvas.width; x++) {
            TGL__GET_PIXEL(canvas, x, y) = pixel;
        }
    }
}

TGLDEF TGL_MAYBE_UNUSED void tgl__draw(tgl__canvas_t canvas) {
    tgl__move_cursor(0, 0);
    int64_t y = 0;
    for (; y < canvas.height - 1; y++) {
        for (int64_t x = 0; x < canvas.width; x++) {
            tgl__term_pixel_t pixel = TGL__GET_PIXEL(canvas, x, y);
            fputs(pixel.background_color, stdout);
            fputs(pixel.foreground_color, stdout);
            putchar(pixel.value);
        }
        fputs("\n", stdout);
    }

    for (int64_t x = 0; x < canvas.width; x++) {
        tgl__term_pixel_t pixel = TGL__GET_PIXEL(canvas, x, y);
        fputs(pixel.background_color, stdout);
        fputs(pixel.foreground_color, stdout);
        putchar(pixel.value);
    }
    fputs(ESC"[0m", stdout);
    fflush(stdout);
}

TGLDEF TGL_MAYBE_UNUSED void
tgl__draw_rect(tgl__canvas_t canvas, int64_t x, int64_t y, int64_t w, int64_t h, tgl__term_pixel_t pixel) {
    int64_t x1, x2, y1, y2;

    if (!tgl__normalize_rect(x, y, w, h, canvas.width, canvas.height, &x1, &x2, &y1, &y2))
        return;

    for (int64_t ix = x1; ix <= x2; ix++) {
        for (int64_t iy = y1; iy <= y2; iy++) {
            TGL__GET_PIXEL(canvas, ix, iy) = pixel;
        }
    }
}

TGLDEF TGL_MAYBE_UNUSED void
tgl__draw_line(tgl__canvas_t canvas, int64_t x1, int64_t y1, int64_t x2, int64_t y2, tgl__term_pixel_t pixel) {
    int64_t dx = x2 - x1;
    int64_t dy = y2 - y1;

    if (dx == 0 && dy == 0) {
        if (tgl__is_inbounds(canvas, x1, y1)) {
            TGL__GET_PIXEL(canvas, x1, y1) = pixel;
        }
        return;
    }

    if (TGL__ABS(int64_t, dx) > TGL__ABS(int64_t, dy)) {
        if (x1 > x2) {
            TGL__SWAP(int64_t, x1, x2);
            TGL__SWAP(int64_t, y1, y2);
        }

        for (int64_t x = x1; x <= x2; x++) {
            int64_t y = dy * (x - x1) / dx + y1;
            if (tgl__is_inbounds(canvas, x, y)) {
                TGL__GET_PIXEL(canvas, x, y) = pixel;
            }
        }
    } else {
        if (y1 > y2) {
            TGL__SWAP(int64_t, x1, x2);
            TGL__SWAP(int64_t, y1, y2);
        }

        for (int64_t y = y1; y <= y2; ++y) {
            int64_t x = dx * (y - y1) / dy + x1;
            if (tgl__is_inbounds(canvas, x, y)) {
                TGL__GET_PIXEL(canvas, x, y) = pixel;
            }
        }
    }
}


TGLDEF TGL_MAYBE_UNUSED bool tgl__is_inbounds(tgl__canvas_t canvas, int64_t x, int64_t y) {
    return 0 <= x && x < (int64_t) canvas.width && 0 <= y && y < (int64_t) canvas.height;
}

TGLDEF TGL_MAYBE_UNUSED void
tgl__draw_ellipse(tgl__canvas_t canvas, int64_t x, int64_t y, int64_t r1, int64_t r2, tgl__term_pixel_t pixel) {
    int64_t wx, wy;
    int64_t thresh;
    int64_t asq = r1 * r1; // TODO: OVERFLOW CHECK
    int64_t bsq = r2 * r2;
    int64_t xa, ya;
    int64_t ix = x, iy = y;
    int64_t ir1 = r1, ir2 = r2;

    TGL__GET_PIXEL(canvas, ix, y + r2) = pixel;
    TGL__GET_PIXEL(canvas, ix, y - r2) = pixel;

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


        TGL__GET_PIXEL(canvas, ix + wx, iy - wy) = pixel;
        TGL__GET_PIXEL(canvas, ix - wx, iy - wy) = pixel;
        TGL__GET_PIXEL(canvas, ix + wx, iy + wy) = pixel;
        TGL__GET_PIXEL(canvas, ix - wx, iy + wy) = pixel;
    }

    TGL__GET_PIXEL(canvas, (x + r1), iy) = pixel;
    TGL__GET_PIXEL(canvas, (x - r1), iy) = pixel;

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

        TGL__GET_PIXEL(canvas, ix + wx, iy - wy) = pixel;
        TGL__GET_PIXEL(canvas, ix - wx, iy - wy) = pixel;
        TGL__GET_PIXEL(canvas, ix + wx, iy + wy) = pixel;
        TGL__GET_PIXEL(canvas, ix - wx, iy + wy) = pixel;
    }
}

#endif

// UNDEF INTERNAL MACROS ---------------------------------------------------------------
#undef OS_LINUX
#undef OS_WINDOWS
#undef TGLDEF
#undef ESC

#endif
