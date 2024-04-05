#ifndef TGL_LIB_H_
#define TGL_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TGLDEF
#define TGLDEF static inline
#endif

#define ESC "\033"
#if defined(__GNUC__) || defined(__clang__)
#define TGL_MAYBE_UNUSED __attribute__((unused))
#else
#define TGL_MAYBE_UNUSED
#endif

// Platform-specific includes and definitions
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

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>


#if !defined(__cplusplus) && __STDC_VERSION__ <= 201710L
#include <stdbool.h>
#endif

typedef struct {
    uint16_t width;
    uint16_t height;
} tgl__term_dim_t;

typedef struct {
    char8_t foreground_color;
    char8_t background_color;
    char8_t value;
} tgl__term_pixel_t;

typedef struct {
    uint16_t width;
    uint16_t height;
    tgl__term_pixel_t *arr;
} tgl__canvas_t;

TGLDEF TGL_MAYBE_UNUSED bool tgl__supports_color(void);

TGLDEF TGL_MAYBE_UNUSED void tgl__clear_terminal(void);

TGLDEF TGL_MAYBE_UNUSED void tgl__hide_cursor(void);

TGLDEF TGL_MAYBE_UNUSED void tgl__show_cursor(void);

TGLDEF TGL_MAYBE_UNUSED tgl__term_dim_t tgl__get_term_size(void);

TGLDEF TGL_MAYBE_UNUSED void tgl__move_cursor(uint16_t x, uint16_t y);

TGLDEF TGL_MAYBE_UNUSED void tgl__draw_pixel(tgl__canvas_t *canvas, uint16_t x, uint16_t y, tgl__term_pixel_t pixel);

TGLDEF TGL_MAYBE_UNUSED void
tgl__draw_line(tgl__canvas_t *canvas, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, tgl__term_pixel_t pixel);

#ifdef __cplusplus
}
#endif

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

TGLDEF tgl__term_dim_t tgl__get_term_size(void) {
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

TGLDEF TGL_MAYBE_UNUSED void tgl__move_cursor(uint16_t x, uint16_t y) {
    printf(ESC "[%hu;%huH", x, y);
}

#endif

#undef OS_LINUX
#undef OS_WINDOWS
#undef TGLDEF
#undef ESC

#endif
