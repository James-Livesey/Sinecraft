#include <gint/gint.h>
#include <gint/clock.h>
#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keyboard.h>

#include "credits.h"

extern bopti_image_t img_logo;

void renderLine(char* text, int* linePos) {
    dtext(0, *linePos, C_BLACK, text);

    (*linePos) += 8;
}

bool shouldStop(int linePos) {
    return linePos <= 63 - 4;
}

void showCredits() {
    int scrollPos = 0;
    unsigned int scrollTick = 0;
    bool autoScroll = true;

    while (true) {
        int linePos = 64 - scrollPos;

        dclear(C_WHITE);

        dimage((128 - img_logo.width) / 2, linePos, &img_logo);

        linePos += img_logo.height + 8;

        // Width:   _____________________
        renderLine("    MUJANG TEAM", &linePos);
        renderLine("    ===========", &linePos);
        renderLine("", &linePos);
        renderLine(" Creator and Dev", &linePos);
        renderLine("       JAMES LIVESEY", &linePos);
        renderLine("", &linePos);
        renderLine(" Mathematical Help", &linePos);
        renderLine("           BEN GROAT", &linePos);
        renderLine("", &linePos);
        renderLine(" Tester (who hasn't", &linePos);
        renderLine(" played the game)", &linePos);
        renderLine("           SAM HONEY", &linePos);
        renderLine("", &linePos);
        renderLine(" That Guy", &linePos);
        renderLine("         ISAAC BETTS", &linePos);
        renderLine("", &linePos);
        renderLine(" Existed During", &linePos);
        renderLine(" Development", &linePos);
        renderLine("    DANIEL GRIFFITHS", &linePos);
        renderLine("", &linePos);
        renderLine("    OTHER PEOPLE", &linePos);
        renderLine("    ============", &linePos);
        renderLine("", &linePos);
        renderLine(" Based on a Vaguely", &linePos);
        renderLine(" Related Game by", &linePos);
        renderLine("      MARKUS PERSSON", &linePos);
        renderLine("", &linePos);
        renderLine(" Bigoted Billionaire", &linePos);
        renderLine("      MARKUS PERSSON", &linePos);
        renderLine("", &linePos);
        renderLine("    SPECIAL THANKS", &linePos);
        renderLine("    ==============", &linePos);
        renderLine("", &linePos);
        renderLine(" fxsdk Maintainer", &linePos);
        renderLine("        Lephenixnoir", &linePos);
        renderLine("  (planet-casio.com)", &linePos);
        renderLine("", &linePos);
        renderLine("", &linePos);
        renderLine("", &linePos);
        renderLine(" jamesl.me/sinecraft", &linePos);
        renderLine("", &linePos);
        renderLine(" Copyright (C) James", &linePos);
        renderLine(" Livesey. All Rights", &linePos);
        renderLine(" Reserved.", &linePos);

        drect(0, 0, 127, 9, C_WHITE);
        dtext(2, 1, C_BLACK, "Credits");
        dhline(10, C_BLACK);

        if (shouldStop(linePos)) {
            autoScroll = false;
        }

        dupdate();

        clearevents();

        if (keydown(KEY_UP)) {
            autoScroll = false;

            if (scrollPos > 0) {
                scrollPos -= 4;
            }
        }

        if (keydown(KEY_DOWN)) {
            autoScroll = false;

            if (shouldStop(linePos)) {
                continue;
            }

            scrollPos += 4;
        }

        if (keydown(KEY_MENU)) {
            gint_osmenu();
        }

        if (keydown(KEY_EXIT)) {
            return;
        }

        sleep_us(1e5);

        if (autoScroll) {
            scrollTick++;

            if (scrollTick % 2 == 0) {
                scrollTick = 0;
                scrollPos++;
            }
        }
    }
}