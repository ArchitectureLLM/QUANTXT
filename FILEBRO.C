#include <dos.h>
#include <string.h>
#include <conio.h>
#include <graph.h>

#include "colors.h"
#include "filebro.h"

char file_list[MAX_FILES][13];
int  file_count = 0;

/* ---------------------------------------------------------
 * Load all *.TXT files in the current directory
 * --------------------------------------------------------- */
void load_txt_file_list(void)
{
    struct find_t f;
    file_count = 0;

    if (_dos_findfirst("*.TXT", _A_NORMAL, &f) == 0) {
        do {
            if (file_count < MAX_FILES) {
                strncpy(file_list[file_count], f.name, 12);
                file_list[file_count][12] = '\0';
                file_count++;
            }
        } while (_dos_findnext(&f) == 0);
    }
}

/* ---------------------------------------------------------
 * XT‑style scrollable file picker
 *
 * Returns:
 *   1  → user selected a file (copied into out_filename)
 *   0  → user cancelled (ESC)
 * --------------------------------------------------------- */
int browse_txt_files(char *out_filename)
{
    int index = 0;
    int top = 0;
    int page = 15;
    int done = 0;

    load_txt_file_list();
    if (file_count == 0)
        return 0;   /* no files → cancel */

    while (!done) {
        int i;

        _clearscreen(_GCLEARSCREEN);

        _settextcolor(COL_HEADER);
        _settextposition(1,1);
        _outtext("SELECT SCENARIO FILE (.TXT)");
        _settextposition(2,1);
        _outtext("----------------------------------------");

        /* draw visible window */
        for (i = 0; i < page && (top + i) < file_count; i++) {
            int row = 4 + i;
            int idx = top + i;

            if (idx == index)
                _settextcolor(COL_WARNING);
            else
                _settextcolor(COL_NORMAL);

            _settextposition(row, 4);
            _outtext(file_list[idx]);
        }

        _settextcolor(COL_FOOTER);
        _settextposition(22,1);
        _outtext("UP/DOWN to select, ENTER to load, ESC to cancel");

        switch (getch()) {

        case 0:
        case 224:
            switch (getch()) {

            case 72: /* up */
                if (index > 0) index--;
                if (index < top) top = index;
                break;

            case 80: /* down */
                if (index < file_count - 1) index++;
                if (index >= top + page) top = index - page + 1;
                break;
            }
            break;

        case 13: /* ENTER */
            strncpy(out_filename, file_list[index], 12);
            out_filename[12] = '\0';
            return 1;

        case 27: /* ESC */
            return 0;
        }
    }

    return 0;
}
