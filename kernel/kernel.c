void kernel_main(void) {
    volatile unsigned short* vga = (unsigned short*)0xB8000;
    const char* msg = "MyOS v0.1 - UFG 2025 | Stefano, Bryan, Kenneth";
    int i;
    for (i = 0; msg[i] != 0; i++) {
        vga[i] = (unsigned short)msg[i] | (0x0A << 8);
    }
    for(;;) {}
}
