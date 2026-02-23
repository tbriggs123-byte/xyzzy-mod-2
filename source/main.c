#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include "source/mini-seeprom.h"

// Your buffer to hold the 256 bytes of SEEPROM data
u8 seeprom_data[256];
int cursor = 0x88; // Default to the Boot2 version offset

void draw_editor() {
    printf("\x1b[2;0H"); // Move cursor to top
    printf("--- Wii SEEPROM Hex Editor ---\n");
    printf("Use D-Pad to move, A/B to change, PLUS to Write, HOME to Exit\n\n");
    
    for (int i = 0; i < 256; i++) {
        if (i == cursor) printf("\x1b[30;47m%02X\x1b[0m ", seeprom_data[i]); // Invert colors for cursor
        else printf("%02X ", seeprom_data[i]);
        
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\nCurrently editing offset: 0x%02X\n", cursor);
    printf("Boot2 Version Byte is at 0x88\n");
}

int main(int argc, char **argv) {
    VIDEO_Init();
    WPAD_Init();
    
    void *rmode = VIDEO_GetPreferredMode(NULL);
    void *xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();

    // Initial Read
    // seeprom_read(0, 256, seeprom_data); 

    while(1) {
        WPAD_ScanPads();
        u32 pressed = WPAD_ButtonsDown(0);

        if (pressed & WPAD_BUTTON_HOME) exit(0);

        // Navigation
        if (pressed & WPAD_BUTTON_RIGHT) cursor = (cursor + 1) % 256;
        if (pressed & WPAD_BUTTON_LEFT)  cursor = (cursor - 1 + 256) % 256;
        if (pressed & WPAD_BUTTON_DOWN)  cursor = (cursor + 16) % 256;
        if (pressed & WPAD_BUTTON_UP)    cursor = (cursor - 16 + 256) % 256;

        // Editing
        if (pressed & WPAD_BUTTON_A) seeprom_data[cursor]++;
        if (pressed & WPAD_BUTTON_B) seeprom_data[cursor]--;

        // Writing (THE DANGER ZONE)
        if (pressed & WPAD_BUTTON_PLUS) {
            printf("\nWRITING TO PHYSICAL SEEPROM... ");
            // seeprom_write(0, 256, seeprom_data);
            printf("DONE!\n");
        }

        draw_editor();
        VIDEO_WaitVSync();
    }
    return 0;
}
