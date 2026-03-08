#ifndef MENU_H
#define MENU_H

#include <bits/stdc++.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "../globals.h"

using namespace std;

// Print semua ruangan
void printAllRuangan() {
    cout << "Daftar Ruangan:" << endl;
    for (const auto& pair : daftarRuangan) {
        cout << "ID: " << pair.second.getId() << ", Nama: " << pair.second.getNamaRuangan() << endl;
    }
}

// ini buat dapetin ukuran terminal, biar bisa adjust tampilan browseRuangan() sesuai ukuran terminal
pair<int,int> getTerminalSize() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return {w.ws_col, w.ws_row};
}

// buat feature 1, pengganti printAllRuangan karena banyak data.
void browseRuangan() {
    vector<pair<string, string>> list;
    for (const auto& pair : daftarRuangan)
        list.push_back({pair.second.getId(), pair.second.getNamaRuangan()});

    int total = list.size();
    int page = 0;

    while (true) {
        system("clear");

        auto [termCols, termRows] = getTerminalSize();

        // Responsive columns: 
        // 1 col if narrow, 2 if medium, 3 if wide
        int cols;
        if (termCols < 60) cols = 1;
        else if (termCols < 120) cols = 2;
        else cols = 3;

        int colWidth = termCols / cols;

        // 5 rows for header dan footer
        int PAGE_SIZE = (termRows - 5) * cols;
        int totalPages = (total + PAGE_SIZE - 1) / PAGE_SIZE;

        cout << GREEN << "Daftar Ruangan (Halaman " << page+1 << "/" << totalPages 
             << ") [" << termCols << "x" << termRows << "]" << RESET << endl;
        cout << string(termCols, '-') << endl;

        int start = page * PAGE_SIZE;
        int end = min(start + PAGE_SIZE, total);

        for (int i = start; i < end; i++) {
            string entry = "ID: " + list[i].first + "  " + list[i].second;

            // Truncator
            if ((int)entry.size() > colWidth - 2)
                entry = entry.substr(0, colWidth - 5) + "...";
            cout << left << setw(colWidth) << entry;
            if ((i - start + 1) % cols == 0) cout << endl;
        }

        // Fill sisa
        if ((end - start) % cols != 0) cout << endl;

        cout << string(termCols, '-') << endl;
        cout << "[N] Next  [P] Prev  [Q] Quit >> ";

        char c; cin >> c; c = tolower(c);
        if (c == 'n' && page < totalPages - 1) page++;
        else if (c == 'p' && page > 0) page--;
        else if (c == 'q') break;
    }
}

#endif