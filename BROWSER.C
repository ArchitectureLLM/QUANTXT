#include <conio.h>
#include <graph.h>
#include <stdio.h>
#include "state.h"
#include "system.h"
#include "scenario.h"
#include "browser.h"
#include "colors.h"


/* ---------------------------------------------------------
 * Simple scenario browser — choose by index
 * --------------------------------------------------------- */
int browse_scenarios(void)
{
    int i;
    int ch;
    int sel = 0;

    _clearscreen(_GCLEARSCREEN);
    _settextcolor(COL_HEADER);
    _settextposition(1, 1);
    _outtext("SCENARIO BROWSER");
    _settextcolor(COL_NORMAL);

    for (;;) {
        int row = 3;

        _clearscreen(_GCLEARSCREEN);
        _settextposition(1, 1);
        _outtext("SCENARIO BROWSER (UP/DOWN, ENTER to select)");

        for (i = 0; i < scenario_count; i++) {
            _settextposition(row + i, 3);
            if (i == sel) {
                _settextcolor(COL_HIGHLIGHT);
                _outtext(">");
            } else {
                _outtext(" ");
            }
            _settextposition(row + i, 5);
            _settextcolor(COL_NORMAL);
            _outtext(scenarios[i].name);
        }

        ch = getch();
        if (ch == 0 || ch == 0xE0) {
            ch = getch();
            if (ch == 72 && sel > 0) sel--;          /* up */
            if (ch == 80 && sel < scenario_count-1) sel++; /* down */
        } else if (ch == 13) {
            return sel;
        } else if (ch == 27) {
            return -1;
        }
    }
}
