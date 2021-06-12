#include <iostream>
#include <ncurses.h>
#include <array>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

void print_rects(int col, int row) {
    mvaddch (0, 0, ACS_ULCORNER);

    for (int x = 1; x < col / 2 - 2; x++) {
        addch (ACS_HLINE);

    }

    addch(ACS_URCORNER);
    addch(ACS_ULCORNER);
    for (int x = col / 2 + 1; x < col - 1; x++) {
        addch(ACS_HLINE);

    }
    addch(ACS_URCORNER);

    for (int y = 1; y < row - 2; y++) {
        mvaddch(y, 0, ACS_VLINE);
    }
    printw("\n");
    addch(ACS_LLCORNER);

    for (int x = 0; x < col / 2 - 3; x++) {
        addch (ACS_HLINE);

    }
    addch(ACS_LRCORNER);

    for (int y = row - 3; y != 0; y--) {
        mvaddch(y, col / 2 - 2, ACS_VLINE);
    }

    for (int y = 1; y < row - 2; y++) {
        mvaddch(y, col / 2 - 1, ACS_VLINE);
    }

    mvaddch (row - 2, col / 2 - 1, ACS_LLCORNER);

    for (int x = col / 2 - 1; x < col - 3; x++) {
        addch(ACS_HLINE);

    }
    addch(ACS_LRCORNER);

    for (int y = row - 3; y != 0; y--) {
        mvaddch(y, col - 2, ACS_VLINE);
    }
}

void print_content(const fs::path &current_path, std::vector<std::string> &folders, std::vector<std::string> &files,
                   std::error_code &ec, int &cursor, int row, int col, int max_row, int max_col) {
    mvprintw(1, col + 1, current_path.c_str());
    int y = row + 2;

    for (auto &p: fs::directory_iterator(current_path, ec)) {
        if (p.is_directory()) {
            folders.push_back(p.path().filename());
        } else if (p.is_regular_file()) {
            files.push_back(p.path().filename());

        } else {

        }
    }
    std::sort(folders.begin(), folders.end());
    std::sort(files.begin(), files.end());
    if (cursor < 0) {
        cursor = 0;
    }
    if (cursor > folders.size() + files.size()) {
        cursor = folders.size() + files.size() - 1;
    }
    int it = 0;
    for (auto &dir:folders) {
        if (it > max_row) {
            break;
        }
        if (it == cursor) {
            attron(A_STANDOUT);
        }
        mvprintw(y, col + 2, dir.c_str());
        y++;
        if (it == cursor) {
            attroff(A_STANDOUT);
        }
        it++;
    }

    attroff(COLOR_PAIR(1));
    attron (COLOR_PAIR(2));
    for (auto &file:files) {
        if (it > max_row) {
            break;
        }
        if (it == cursor) {
            attron(A_STANDOUT);
        }
        mvprintw(y, col + 2, file.c_str());
        y++;
        if (it == cursor) {
            attroff(A_STANDOUT);
        }
        it++;
    }
    attroff(COLOR_PAIR(2));
    attron (COLOR_PAIR(1));
}

void key_board_handle(fs::path &current_path, std::vector<std::string> &folders, int &cursor, bool &is_run,
                      int &active_wnd) {
    auto ch = wgetch(stdscr);

    switch (ch) {
        case 'q':
            is_run = false;
            break;
        case KEY_LEFT:
            current_path = current_path.parent_path();
            break;
        case KEY_UP:
            cursor--;
            break;
        case KEY_DOWN:
            cursor++;
            break;
        case 10:
            if (cursor < folders.size()) {
                current_path = current_path / folders[cursor];
            }
            break;
        case 9:
            if (active_wnd == 0) {
                active_wnd = 1;
            } else {
                active_wnd = 0;
            }
            default:
            break;

    }
}


int main() {
    int row, col;
    bool is_run = true;
    fs::path current_path1 = fs::current_path();
    fs::path current_path2 = fs::current_path();
    int cursor1 = 0;
    int cursor2 = 0;
    std::error_code ec;
    int active_wnd = 0;
    initscr();
    cbreak();
    start_color();
    clear();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    attron (COLOR_PAIR(1));


    while (is_run) {
        getmaxyx (stdscr, row, col);
        clear();
        std::vector<std::string> folders1, files1, folders2, files2;
        print_rects(col, row);
        print_content(current_path1, folders1, files1, ec, cursor1, 0, 0, row - 5, col / 2 - 1);
        print_content(current_path2, folders2, files2, ec, cursor2, 0, col / 2 + 1, row - 5, col);
        if (active_wnd == 0) {
            key_board_handle(current_path1, folders1, cursor1, is_run, active_wnd);
        } else {
            key_board_handle(current_path2, folders2, cursor2, is_run, active_wnd);
        }

        refresh();


    }
    clrtoeol();
    refresh();
    endwin();


    return 0;
}