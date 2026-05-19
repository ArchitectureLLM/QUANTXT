#include <graph.h>
#include <conio.h>
#include <stdio.h>

#include "state.h"
#include "system.h"
#include "scenario.h"
#include "modules.h"
#include "dash.h"
#include "browser.h"
#include "colors.h"
#include "qxcalib.h"
#include "intro.h"
#include "filebro.h"

#define LOGO_LEFT      35
#define LOGO_COL       (LOGO_LEFT / 8)   /* 8px BIOS font width */

#define MENU_WIDTH   40   /* longest menu line */
#define MENU_COL     (LOGO_COL + ( (80 - LOGO_COL) - MENU_WIDTH ) / 2 )

#define MENU_HEIGHT   7
#define MENU_TOP      ((25 - MENU_HEIGHT) / 2)

int main(void)
{
    State         s;
    SystemOut     o;
    int           idx;
    int           ch;
    int           added;
    int           ok;
    char          filename[13];
    QXCalibResult r;
    double        lagged_ai = 0.0;

    _setvideomode(_TEXTC80);
    run_intro();

    for (;;) {
        _clearscreen(_GCLEARSCREEN);
        _settextcolor(COL_HEADER);

        /* Title */
        _settextposition(MENU_TOP, MENU_COL);
        _outtext("QUANTXT v1.1  (IBM PC XT Edition)");

        /* Menu items */
        _settextposition(MENU_TOP + 2, MENU_COL);
        _outtext("1) Browse scenarios");

        _settextposition(MENU_TOP + 3, MENU_COL);
        _outtext("2) Manual input");

        _settextposition(MENU_TOP + 4, MENU_COL);
        _outtext("3) Load scenario file (.TXT)");

        _settextposition(MENU_TOP + 5, MENU_COL);
        _outtext("4) Run calibration (CALIB.TXT)");

        _settextposition(MENU_TOP + 6, MENU_COL);
        _outtext("ESC) Exit");

        /* Prompt */
        _settextposition(MENU_TOP + 8, MENU_COL);
        _outtext("Select option: ");



        ch = getch();

        if (ch == 27) {
            break;
        }
        else if (ch == '1') {
            idx = browse_scenarios();
            if (idx < 0) continue;
            fill_state_from_scenario(&scenarios[idx], &s, lagged_ai);
        }
        else if (ch == '2') {
            manual_input(&s);
        }
        else if (ch == '3') {
            if (browse_txt_files(filename)) {
                added = load_scenario_file_multi(filename);

                _clearscreen(_GCLEARSCREEN);
                _settextposition(1, 1);

                if (added > 0) {
                    _settextcolor(COL_HEADER);
                    _outtext("Scenarios loaded. Entering browser...");
                    _settextcolor(COL_NORMAL);

                    /* brief pause */
                    {
                        volatile long d;
                        for (d = 0; d < 200000L; d++);
                    }

                    idx = browse_scenarios();
                    if (idx >= 0) {
                        fill_state_from_scenario(&scenarios[idx], &s, lagged_ai);
                        compute_system_out(&s, &o);
                        draw_dashboard(&o, &s);
                        lagged_ai = s.ai_capex;
                    }
                } else {
                    _settextcolor(COL_WARNING);
                    _outtext("No scenarios found in file.");
                    _settextcolor(COL_NORMAL);
                    _settextposition(2, 1);
                    _outtext("Check format: name f1 f2 ... f15 per line");
                    _settextposition(3, 1);
                    _outtext("Press any key...");
                    getch();
                }
            }
            continue;
        }
        else if (ch == '4') {
            /* -------------------------------
               RUN CALIBRATION MODE
            ------------------------------- */
            ok = qxcalib_run("CALIB.TXT", "CALIB_RESULT.TXT", &r);

            _clearscreen(_GCLEARSCREEN);
            _settextposition(1, 1);
            _settextcolor(COL_HEADER);

            if (ok == 0) {
                _outtext("Calibration complete.");
                _settextcolor(COL_NORMAL);
                _settextposition(3, 1);
                printf("Rows tested : %d\n",    r.rows);
                printf("Total error : %.6f\n",  r.total_error);
                printf("Mean error  : %.6f\n\n",r.mean_error);
                printf("Results saved to CALIB_RESULT.TXT\n");
            } else {
                _outtext("Calibration failed.");
                _settextcolor(COL_NORMAL);
                _settextposition(3, 1);
                printf("Check CALIB.TXT format.\n");
            }

            printf("\nPress any key to return to menu...");
            getch();
            continue;
        }
        else {
            continue;
        }

        compute_system_out(&s, &o);
        draw_dashboard(&o, &s);

        lagged_ai = s.ai_capex; /* simple lag example */
    }

    _setvideomode(_DEFAULTMODE);
    return 0;
}
