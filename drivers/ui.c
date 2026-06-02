#include "ui.h"
#include "vga.h"
#include "../lib/string.h"

#define W 80
#define H 25

/* Escribe un caracter en posicion absoluta con color */
static void vga_put_at(int row, int col, char c, uint8_t fg, uint8_t bg) {
    volatile uint16_t* mem = (volatile uint16_t*)0xB8000;
    uint8_t attr = (bg << 4) | (fg & 0x0F);
    mem[row * W + col] = (uint16_t)c | ((uint16_t)attr << 8);
}

static void vga_str_at(int row, int col, const char* s, uint8_t fg, uint8_t bg) {
    for (int i = 0; s[i]; i++)
        vga_put_at(row, col + i, s[i], fg, bg);
}

static void fill_row(int row, char c, uint8_t fg, uint8_t bg) {
    for (int i = 0; i < W; i++)
        vga_put_at(row, i, c, fg, bg);
}

/* Delay simple */
static void delay(int n) {
    for (volatile int i = 0; i < n * 100000; i++);
}

void ui_boot_screen() {
    vga_clear();

    /* Fondo negro, letras cian para el ASCII art */
    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);

    vga_str_at(5, 20, " __  __       ___  ____  ", VGA_LIGHT_CYAN, VGA_BLACK);
    vga_str_at(6, 20, "|  \\/  |_   _/ _ \\/ ___| ", VGA_LIGHT_CYAN, VGA_BLACK);
    vga_str_at(7, 20, "| |\\/| | | | | | |\\___ \\ ", VGA_CYAN,       VGA_BLACK);
    vga_str_at(8, 20, "| |  | | |_| | |_| |___) |", VGA_BLUE,      VGA_BLACK);
    vga_str_at(9, 20, "|_|  |_|\\__, |\\___/|____/ ", VGA_DARK_GREY, VGA_BLACK);
    vga_str_at(10,20, "        |___/              ", VGA_DARK_GREY, VGA_BLACK);

    vga_str_at(12, 22, "Universidad Francisco Gavidia", VGA_WHITE, VGA_BLACK);
    vga_str_at(13, 28, "v0.1  -  2025", VGA_LIGHT_GREY, VGA_BLACK);

    /* Barra de carga */
    vga_str_at(16, 20, "Iniciando:", VGA_LIGHT_GREY, VGA_BLACK);
    vga_str_at(16, 31, "[                    ]", VGA_DARK_GREY, VGA_BLACK);

    uint8_t colors[] = {VGA_DARK_GREY, VGA_LIGHT_GREY, VGA_WHITE, VGA_LIGHT_CYAN, VGA_CYAN};
    for (int i = 0; i < 20; i++) {
        uint8_t col = colors[i % 5];
        vga_put_at(16, 32 + i, '#', col, VGA_BLACK);
        delay(1);
    }

    vga_str_at(18, 27, "[ Sistema listo ]", VGA_LIGHT_GREEN, VGA_BLACK);
    delay(8);
    vga_clear();
}

void ui_draw_statusbar() {
    fill_row(0, ' ', VGA_WHITE, VGA_BLUE);
    vga_str_at(0, 1,  "MyOS v0.1", VGA_WHITE,       VGA_BLUE);
    vga_str_at(0, 28, "Universidad Francisco Gavidia", VGA_LIGHT_CYAN, VGA_BLUE);
    vga_str_at(0, 72, "UFG 25", VGA_BROWN, VGA_BLUE);
}

void ui_draw_dock() {
    fill_row(24, ' ', VGA_WHITE, VGA_DARK_GREY);
    vga_str_at(24, 1,  "[ Shell ]", VGA_WHITE,        VGA_DARK_GREY);
    vga_str_at(24, 12, "[ Calc  ]", VGA_LIGHT_GREEN,  VGA_DARK_GREY);
    vga_str_at(24, 23, "[ Files ]", VGA_LIGHT_CYAN,   VGA_DARK_GREY);
    vga_str_at(24, 34, "[ Snake ]", VGA_LIGHT_RED,    VGA_DARK_GREY);
    vga_str_at(24, 45, "[ Info  ]", VGA_BROWN,       VGA_DARK_GREY);
    vga_str_at(24, 65, "type 'help' p/comandos", VGA_DARK_GREY, VGA_DARK_GREY);
}

void ui_draw_desktop() {
    vga_clear();
    ui_draw_statusbar();

    /* Area central - fondo oscuro */
    for (int r = 1; r < 24; r++)
        fill_row(r, ' ', VGA_LIGHT_GREY, VGA_BLACK);

    /* Linea separadora debajo del statusbar */
    fill_row(1, '-', VGA_DARK_GREY, VGA_BLACK);

    ui_draw_dock();
}
