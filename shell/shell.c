#include "shell.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "../lib/string.h"

#define BUFSIZE 256

static char buf[BUFSIZE];
static int  buf_pos = 0;

static void cmd_help() {
    vga_print("Comandos disponibles:\n");
    vga_print("  help    - muestra esta ayuda\n");
    vga_print("  clear   - limpia la pantalla\n");
    vga_print("  info    - informacion del OS\n");
    vga_print("  echo    - imprime un mensaje\n");
    vga_print("  reboot  - reinicia el sistema\n");
}

static void cmd_info() {
    vga_print("MyOS v0.1\n");
    vga_print("Universidad Francisco Gavidia - 2025\n");
    vga_print("Integrantes: Stefano, Bryan, Kenneth, Andres\n");
    vga_print("Arquitectura: x86 32-bit\n");
}

static void cmd_clear() {
    vga_clear();
}

static void cmd_echo(char* args) {
    vga_print(args);
    vga_print("\n");
}

static void cmd_reboot() {
    vga_print("Reiniciando...\n");
    __asm__ volatile ("outb %0, %1" : : "a"((unsigned char)0xFE), "Nd"((unsigned short)0x64));
}

static void execute(char* line) {
    char* cmd  = line;
    char* args = line;

    while (*args && *args != ' ') args++;
    if (*args == ' ') { *args = 0; args++; }
    else args = "";

    if      (!strcmp(cmd, "help"))   cmd_help();
    else if (!strcmp(cmd, "clear"))  cmd_clear();
    else if (!strcmp(cmd, "info"))   cmd_info();
    else if (!strcmp(cmd, "echo"))   cmd_echo(args);
    else if (!strcmp(cmd, "reboot")) cmd_reboot();
    else if (cmd[0] == 0)            return;
    else {
        vga_print("Comando no encontrado: ");
        vga_print(cmd);
        vga_print("\n");
    }
}

void shell_run() {
    vga_print("\nMyOS> ");
    while (1) {
        char c = keyboard_getchar();
        if (!c) continue;

        if (c == '\n') {
            vga_putchar('\n');
            buf[buf_pos] = 0;
            execute(buf);
            buf_pos = 0;
            vga_print("MyOS> ");
        } else if (c == '\b') {
            if (buf_pos > 0) {
                buf_pos--;
                vga_putchar('\b');
                vga_putchar(' ');
                vga_putchar('\b');
            }
        } else if (buf_pos < BUFSIZE - 1) {
            buf[buf_pos++] = c;
            vga_putchar(c);
        }
    }
}
