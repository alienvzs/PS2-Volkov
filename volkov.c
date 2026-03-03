#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <fileXio_rpc.h>
#include <libpad.h>
#include <gsKit.h>
#include <stdio.h>
#include <string.h>

// --- DRIVER BETÖLTÉS (IRX) ---
void load_drivers() {
    SifInitRpc(0);
    // Alapvető driverek a gép ROM-jából (Memóriakártya + Kontroller)
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:MCMAN", 0, NULL);
    SifLoadModule("rom0:MCSERV", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);
    // USB driverek (ha uLaunchELF-ből indítod, ezek már a RAM-ban lesznek)
    fileXioInit();
}

// --- KEZELŐFELÜLET VÁLTOZÓK ---
int cursor_pos = 0;
int active_panel = 0; // 0: Bal (mc0), 1: Jobb (mass)
char* files[20];      // Maximum 20 fájl listázása az egyszerűség kedvéért

// --- GRAFIKA ---
GSGLOBAL *gsGlobal;
#define VC_BLUE   GS_SETREG_RGBAQ(0, 0, 128, 0, 0x00)
#define VC_WHITE  GS_SETREG_RGBAQ(255, 255, 255, 0, 0x00)
#define VC_CYAN   GS_SETREG_RGBAQ(0, 255, 255, 0, 0x00)

void draw_panels() {
    gsKit_clear(gsGlobal, VC_BLUE);
    // Bal panel keret
    gsKit_prim_line(gsGlobal, 20, 40, 300, 40, 1, VC_WHITE);
    gsKit_prim_line(gsGlobal, 20, 400, 300, 400, 1, VC_WHITE);
    // Jobb panel keret
    gsKit_prim_line(gsGlobal, 320, 40, 600, 40, 1, VC_WHITE);
    gsKit_prim_line(gsGlobal, 320, 400, 600, 400, 1, VC_WHITE);
    
    // Kurzor rajzolása (Kijelölés)
    int y_pos = 60 + (cursor_pos * 15);
    int x_start = (active_panel == 0) ? 25 : 325;
    gsKit_prim_sprite(gsGlobal, x_start, y_pos, x_start + 270, y_pos + 15, 1, VC_CYAN);
}

// --- FŐPROGRAM ---
int main() {
    load_drivers();
    
    gsGlobal = gsKit_init_global();
    gsKit_init_screen(gsGlobal);

    // Kontroller puffer
    static char padBuf[256] __attribute__((aligned(64)));
    padInit(0);
    padPortOpen(0, 0, padBuf);

    struct padButtonStatus buttons;

    while(1) {
        draw_panels();

        // Gombkezelés (D-PAD)
        if(padRead(0, 0, &buttons) != 0) {
            uint32_t paddata = 0xffff ^ buttons.btns;

            if(paddata & PAD_DOWN) {
                cursor_pos++;
                if(cursor_pos > 19) cursor_pos = 0;
                scr_printf("Le\n"); // Debug infó
            }
            if(paddata & PAD_UP) {
                cursor_pos--;
                if(cursor_pos < 0) cursor_pos = 19;
            }
            if(paddata & PAD_LEFT || paddata & PAD_RIGHT) {
                active_panel = !active_panel; // Panel váltás (TAB funkció)
            }
            if(paddata & PAD_SQUARE) { // F5 Másolás szimuláció
                // Itt hívnánk meg a copy_file függvényt
            }
        }

        gsKit_queue_exec(gsGlobal);
        gsKit_sync_flip(gsGlobal);
        
        // Kis várakozás, hogy ne pörögjön túl a kurzor
        for(int i=0; i<100000; i++) asm("nop");
    }
    return 0;
}
