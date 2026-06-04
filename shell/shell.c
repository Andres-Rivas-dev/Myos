#include "shell.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "../drivers/ui.h"
#include "../lib/string.h"

#define BUFSIZE  256
#define HIST_MAX 10

static char buf[BUFSIZE];
static int  buf_len = 0;
static int  buf_cur = 0;   /* indice logico en buf */

static char history[HIST_MAX][BUFSIZE];
static int  hist_count = 0;
static int  hist_idx   = -1;

static void hist_push(const char* cmd) {
    if (cmd[0] == 0) return;
    if (hist_count > 0 && !strcmp(history[(hist_count-1) % HIST_MAX], cmd)) return;
    int slot = hist_count % HIST_MAX;
    int i;
    for (i = 0; cmd[i] && i < BUFSIZE-1; i++) history[slot][i] = cmd[i];
    history[slot][i] = 0;
    hist_count++;
}

/*
 * Redibuja buf[buf_cur..buf_len] en pantalla desde la posicion actual
 * del cursor hardware, luego borra el caracter extra si el buffer se
 * acorto, y deja el cursor hardware apuntando a buf_cur.
 */
static void redraw_from_cursor() {
    /* guardar columna de inicio (= donde esta buf_cur ahora) */
    int start_col = vga_get_col();
    /* pintar buf[buf_cur .. buf_len-1] */
    for (int i = buf_cur; i < buf_len; i++) vga_putchar(buf[i]);
    /* borrar un caracter extra por si el buffer se acorto */
    vga_putchar(' ');
    /* retroceder hasta start_col */
    int end_col = vga_get_col();   /* columna despues de pintar + espacio */
    for (int i = end_col; i > start_col; i--) vga_putchar('\b');
}

/* Borra visualmente toda la entrada actual y resetea buf */
static void clear_input() {
    /* retroceder hasta buf_cur */
    for (int i = 0; i < buf_cur; i++) vga_putchar('\b');
    /* borrar buf_len caracteres */
    for (int i = 0; i < buf_len; i++) vga_putchar(' ');
    /* volver al inicio */
    for (int i = 0; i < buf_len; i++) vga_putchar('\b');
    buf_len = 0; buf_cur = 0; buf[0] = 0;
}

static void set_input(const char* src) {
    clear_input();
    int i;
    for (i = 0; src[i] && i < BUFSIZE-1; i++) {
        buf[i] = src[i];
        vga_putchar(src[i]);
    }
    buf[i] = 0; buf_len = i; buf_cur = i;
}

/* ── Apps ─────────────────────────────────────────── */
static void cmd_calc(char* args) {
    if (!args || args[0] == 0) {
        vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
        vga_print("\n  === CALCULADORA ===\n");
        vga_print("  Uso: calc 10 + 5\n");
        vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
        return;
    }
    int a = 0, b = 0, i = 0;
    char op = 0;
    while (args[i] >= '0' && args[i] <= '9') a = a*10 + (args[i++]-'0');
    while (args[i] == ' ') i++;
    op = args[i++];
    while (args[i] == ' ') i++;
    while (args[i] >= '0' && args[i] <= '9') b = b*10 + (args[i++]-'0');
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_print("  = ");
    int result = 0, valid = 1;
    if      (op == '+') result = a + b;
    else if (op == '-') result = a - b;
    else if (op == '*') result = a * b;
    else if (op == '/') { if (b) result=a/b; else { vga_print("Error: div/0\n"); valid=0; } }
    else { vga_print("Op invalida. Usa + - * /\n"); valid=0; }
    if (valid) {
        if (result < 0) { vga_putchar('-'); result = -result; }
        char tmp[16]; int ti = 0;
        if (result == 0) tmp[ti++] = '0';
        else { int r=result; while(r){tmp[ti++]='0'+(r%10);r/=10;} }
        for (int j=ti-1;j>=0;j--) vga_putchar(tmp[j]);
        vga_putchar('\n');
    }
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
}

static void cmd_files() {
    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
    vga_print("\n  === SISTEMA DE ARCHIVOS ===\n");
    vga_print("  /\n");
    vga_print("  +-- boot/      (bootloader GRUB)\n");
    vga_print("  +-- kernel/    (gdt, idt, kernel principal)\n");
    vga_print("  +-- drivers/   (vga, keyboard, ui)\n");
    vga_print("  +-- shell/     (interprete de comandos)\n");
    vga_print("  +-- lib/       (string, utilidades)\n");
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
}

static void cmd_snake() {
    vga_set_color(VGA_LIGHT_RED, VGA_BLACK);
    vga_print("\n  === SNAKE ===\n");
    vga_print("  ####################\n");
    vga_print("  #                  #\n");
    vga_print("  #   >>>>v          #\n");
    vga_print("  #       v  *       #\n");
    vga_print("  #       v          #\n");
    vga_print("  #       >>>>       #\n");
    vga_print("  #                  #\n");
    vga_print("  ####################\n");
    vga_print("  Score: 10\n");
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
}

static void cmd_help() {
    vga_set_color(VGA_BROWN, VGA_BLACK);
    vga_print("\n  Comandos disponibles:\n");
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    vga_print("  help          - muestra esta ayuda\n");
    vga_print("  clear         - limpia la pantalla\n");
    vga_print("  info          - informacion del OS\n");
    vga_print("  echo [texto]  - imprime un mensaje\n");
    vga_print("  calc [a op b] - calculadora (ej: calc 5 + 3)\n");
    vga_print("  files         - explorador de archivos\n");
    vga_print("  snake         - mini juego\n");
    vga_print("  color [n]     - cambia color (0-15)\n");
    vga_print("  history       - ver historial\n");
    vga_print("  reboot        - reinicia el sistema\n");
    vga_print("\n  Flechas: izq/der mueven cursor, arriba/abajo historial\n");
}

static void cmd_info() {
    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
    vga_print("\n  === MyOS v0.1 ===\n");
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    vga_print("  Universidad Francisco Gavidia - 2025\n");
    vga_print("  Integrantes: Stefano, Bryan, Andres\n");
    vga_print("  Arquitectura : x86 32-bit\n");
    vga_print("  Bootloader   : GRUB Multiboot\n");
    vga_print("  Kernel       : C + Assembly\n");
    vga_print("  Drivers      : VGA text mode, PS/2 Keyboard\n");
    vga_print("  Shell        : Interactivo con historial\n");
}

static void cmd_history() {
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    vga_print("\n  Historial:\n");
    int start = hist_count > HIST_MAX ? hist_count - HIST_MAX : 0;
    for (int i = start; i < hist_count; i++) {
        vga_print("  ");
        vga_putchar('0' + ((i+1) % 10));
        vga_print(": ");
        vga_print(history[i % HIST_MAX]);
        vga_putchar('\n');
    }
}

static void cmd_color(char* args) {
    if (!args || args[0] == 0) { vga_print("  Uso: color [0-15]\n"); return; }
    int n = args[0] - '0';
    if (args[1] >= '0' && args[1] <= '9') n = n*10 + (args[1]-'0');
    if (n < 0 || n > 15) { vga_print("  Color 0-15\n"); return; }
    vga_set_color((uint8_t)n, VGA_BLACK);
    vga_print("  Color cambiado!\n");
}

static void execute(char* line) {
    char* cmd = line, *args = line;
    while (*args && *args != ' ') args++;
    if (*args == ' ') { *args = 0; args++; } else args = "";

    if      (!strcmp(cmd, "help"))    cmd_help();
    else if (!strcmp(cmd, "clear"))   { vga_clear(); ui_draw_desktop(); }
    else if (!strcmp(cmd, "info"))    cmd_info();
    else if (!strcmp(cmd, "echo"))    { vga_set_color(VGA_WHITE,VGA_BLACK); vga_print("  "); vga_print(args); vga_print("\n"); vga_set_color(VGA_LIGHT_GREY,VGA_BLACK); }
    else if (!strcmp(cmd, "calc"))    cmd_calc(args);
    else if (!strcmp(cmd, "files"))   cmd_files();
    else if (!strcmp(cmd, "snake"))   cmd_snake();
    else if (!strcmp(cmd, "color"))   cmd_color(args);
    else if (!strcmp(cmd, "history")) cmd_history();
    else if (!strcmp(cmd, "reboot"))  {
        vga_print("  Reiniciando...\n");
        __asm__ volatile ("outb %0, %1" : : "a"((unsigned char)0xFE), "Nd"((unsigned short)0x64));
    }
    else if (cmd[0] == 0) return;
    else {
        vga_set_color(VGA_LIGHT_RED, VGA_BLACK);
        vga_print("  Comando no encontrado: ");
        vga_print(cmd);
        vga_print("\n");
        vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    }
}

void shell_run() {
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    vga_print("\n  Bienvenido a MyOS. Escribe 'help' para ver comandos.\n");

    while (1) {
        hist_idx = -1;
        buf_len  = 0;
        buf_cur  = 0;
        buf[0]   = 0;

        vga_putchar('\n');
        vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
        vga_print("  MyOS> ");
        vga_set_color(VGA_WHITE, VGA_BLACK);

        while (1) {
            char c = keyboard_getchar();
            if (!c) continue;

            if (c == '\n') {
                /* avanzar cursor al final antes de saltar de linea */
                for (int i = buf_cur; i < buf_len; i++) vga_putchar(buf[i]);
                vga_putchar('\n');
                buf[buf_len] = 0;
                hist_push(buf);
                execute(buf);
                break;

            } else if (c == KEY_LEFT) {
                if (buf_cur > 0) {
                    buf_cur--;
                    vga_putchar('\b');
                }

            } else if (c == KEY_RIGHT) {
                if (buf_cur < buf_len) {
                    vga_putchar(buf[buf_cur]);
                    buf_cur++;
                }

            } else if (c == KEY_UP) {
                int avail = hist_count > HIST_MAX ? HIST_MAX : hist_count;
                if (hist_idx + 1 < avail) {
                    hist_idx++;
                    set_input(history[(hist_count - 1 - hist_idx) % HIST_MAX]);
                }

            } else if (c == KEY_DOWN) {
                if (hist_idx > 0) {
                    hist_idx--;
                    set_input(history[(hist_count - 1 - hist_idx) % HIST_MAX]);
                } else if (hist_idx == 0) {
                    hist_idx = -1;
                    clear_input();
                }

            } else if (c == '\b') {
                if (buf_cur > 0) {
                    /* eliminar caracter en buf_cur-1 */
                    for (int i = buf_cur-1; i < buf_len-1; i++) buf[i] = buf[i+1];
                    buf_len--; buf_cur--;
                    buf[buf_len] = 0;
                    vga_putchar('\b');
                    redraw_from_cursor();
                }

            } else if (buf_len < BUFSIZE - 1) {
                /* insertar en buf_cur */
                for (int i = buf_len; i > buf_cur; i--) buf[i] = buf[i-1];
                buf[buf_cur] = c;
                buf_len++; buf_cur++;
                buf[buf_len] = 0;
                /* pintar c y redibujar el resto */
                vga_putchar(c);
                redraw_from_cursor();
            }
        }
    }
}
