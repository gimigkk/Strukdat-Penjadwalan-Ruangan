#ifndef MENU_H
#define MENU_H

#include <bits/stdc++.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include "../globals.h"
#include "../latency.h"

using namespace std;

// buat raw mode biar input langsung kebaca tanpa nunggu Enter
void enableRawMode(termios& orig) {
    termios raw = orig;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode(termios& orig) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
}

// baca input keyboard, return 'n', 'p', 'q'
// works buat arrow key maupun N/P/Q biasa
int readKey() {
    char c;
    read(STDIN_FILENO, &c, 1);
    if (c == '\033') {          // escape sequence, berarti arrow key
        char seq[2];
        read(STDIN_FILENO, &seq[0], 1);
        read(STDIN_FILENO, &seq[1], 1);
        if (seq[0] == '[') {
            if (seq[1] == 'C') return 'n'; // right arrow → next
            if (seq[1] == 'D') return 'p'; // left arrow  → prev
            if (seq[1] == 'B') return 'n'; // down arrow  → next
            if (seq[1] == 'A') return 'p'; // up arrow    → prev
        }
    }
    return tolower(c);
}

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
    {
        // Ukur waktu traversal seluruh map ruangan
        ScopeTimer t("map::scan (browseRuangan)");
        for (const auto& pair : daftarRuangan)
            list.push_back({pair.second.getId(), pair.second.getNamaRuangan()});
    }

    int total = list.size();
    if (total == 0) {
        cout << RED << "Tidak ada ruangan tersedia." << RESET << endl;
        return;
    }
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
        cout << GRAY << "[←/→/↑/↓] atau [N/P] Navigate  [Q] Quit" << RESET << endl;
        cout.flush(); // flush dulu sebelum system("clear") di iterasi berikutnya

        termios orig;
        tcgetattr(STDIN_FILENO, &orig);
        enableRawMode(orig);
        int c = readKey();
        disableRawMode(orig);

        if (c == 'n' && page < totalPages - 1) page++;
        else if (c == 'p' && page > 0) page--;
        else if (c == 'q') break;
    }
}

// pagination buat jadwal di suatu ruangan
// multi-column, tiap jadwal tetap multi-line, di-render side by side
void browseJadwal(const Ruangan& ruangan) {
    vector<const Jadwal*> list;
    for (const auto& pair : ruangan.getJadwal())
        list.push_back(&pair.second);

    // sort dari terbaru ke terlama berdasarkan waktu mulai
    sort(list.begin(), list.end(), [](const Jadwal* a, const Jadwal* b) {
        return a->getMulai() > b->getMulai();
    });

    int total = list.size();
    int page = 0;

    time_t now = time(nullptr);

    while (true) {
        system("clear");

        auto [termCols, termRows] = getTerminalSize();

        // responsive columns
        int cols;
        if (termCols < 80) cols = 1;
        else if (termCols < 160) cols = 2;
        else cols = 3;

        int colWidth = termCols / cols;

        // tiap entry = 5 baris, sisain 3 buat header/footer
        int rowsPerPage = (termRows - 3) / 5;
        if (rowsPerPage < 1) rowsPerPage = 1;
        int PAGE_SIZE = rowsPerPage * cols;
        int totalPages = (total + PAGE_SIZE - 1) / PAGE_SIZE;

        cout << GREEN << "Jadwal Ruangan: " << ruangan.getNamaRuangan()
             << " (Halaman " << page+1 << "/" << totalPages << ")" << RESET << endl;
        cout << string(termCols, '-') << endl;

        int start = page * PAGE_SIZE;
        int end = min(start + PAGE_SIZE, total);

        // render per "row" - tiap row = cols entry side by side
        for (int row = 0; row < rowsPerPage; row++) {
            // kumpulin entry di row ini
            vector<int> indices;
            for (int col = 0; col < cols; col++) {
                int idx = start + row * cols + col;
                if (idx < end) indices.push_back(idx);
            }
            if (indices.empty()) break;

            // build tiap baris dari entry side by side
            // format: ID, Kegiatan, Mulai, Selesai, ---
            auto getJadwalLine = [&](int idx, int lineNum) -> string {
                const Jadwal* j = list[idx];
                switch(lineNum) {
                    case 0: return "ID      : " + j->getIdJadwal();
                    case 1: return "Kegiatan: " + j->getNamaKegiatan();
                    case 2: return "Mulai   : " + formatTime(j->getMulai());
                    case 3: return "Selesai : " + formatTime(j->getSelesai());
                    case 4: return "";
                    default: return "";
                }
            };

            bool isLastRow = (start + (row + 1) * cols >= end);
            for (int lineNum = 0; lineNum < 5; lineNum++) {
                // skip baris kosong kalau ini row terakhir di halaman
                if (lineNum == 4 && isLastRow) continue;
                for (int col = 0; col < (int)indices.size(); col++) {
                    int idx = indices[col];
                    const Jadwal* j = list[idx];
                    string color = (j->getSelesai() < now) ? GRAY : RESET;
                    string line = getJadwalLine(idx, lineNum);

                    // truncate kalau kepanjangan
                    if ((int)line.size() > colWidth - 2)
                        line = line.substr(0, colWidth - 5) + "...";

                    // pad dulu baru color, biar setw ga kerusak sama ANSI codes
                    ostringstream padded;
                    padded << left << setw(colWidth) << line;
                    cout << color << padded.str() << RESET;
                }
                cout << endl;
            }
        }

        cout << string(termCols, '-') << endl;
        cout << GRAY << "[←/→/↑/↓] atau [N/P] Navigate  [Q] Quit" << RESET << endl;
        cout.flush();

        // langsung keluar kalau tidak ada data
        if (total == 0) break;

        termios orig;
        tcgetattr(STDIN_FILENO, &orig);
        enableRawMode(orig);
        int c = readKey();
        disableRawMode(orig);

        if (c == 'n' && page < totalPages - 1) page++;
        else if (c == 'p' && page > 0) page--;
        else if (c == 'q') break;
    }
}

void browseJadwalByDate(const Ruangan& ruangan, time_t targetDate) {
    vector<const Jadwal*> list;
    tm* targetTm = localtime(&targetDate);
    int targetYear = targetTm->tm_year;
    int targetMonth = targetTm->tm_mon;
    int targetDay = targetTm->tm_mday;

    for (const auto& pair : ruangan.getJadwal()) {
        const Jadwal& data = pair.second;
        time_t mulai = data.getMulai();
        tm* jadwalTm = localtime(&mulai);
        if (jadwalTm->tm_year == targetYear &&
            jadwalTm->tm_mon == targetMonth &&
            jadwalTm->tm_mday == targetDay) {
            list.push_back(&data);
        }
    }

    sort(list.begin(), list.end(), [](const Jadwal* a, const Jadwal* b) {
        return a->getMulai() > b->getMulai();
    });

    int total = list.size();
    int page = 0;
    time_t now = time(nullptr);

    while (true) {
        system("clear");

        auto [termCols, termRows] = getTerminalSize();

        int cols;
        if (termCols < 80) cols = 1;
        else if (termCols < 160) cols = 2;
        else cols = 3;

        int colWidth = termCols / cols;

        int rowsPerPage = (termRows - 3) / 5;
        if (rowsPerPage < 1) rowsPerPage = 1;
        int PAGE_SIZE = rowsPerPage * cols;
        int totalPages = max(1, (total + PAGE_SIZE - 1) / PAGE_SIZE);

        cout << GREEN << "Jadwal Ruangan: " << ruangan.getNamaRuangan()
             << " pada " << formatDate(targetDate)
             << " (Halaman " << page+1 << "/" << totalPages << ")" << RESET << endl;
        cout << string(termCols, '-') << endl;

        if (total == 0) {
            cout <<  "Tidak ada jadwal pada tanggal " << formatDate(targetDate) << "." << RESET << endl;
        } else {
            int start = page * PAGE_SIZE;
            int end = min(start + PAGE_SIZE, total);

            for (int row = 0; row < rowsPerPage; row++) {
                vector<int> indices;
                for (int col = 0; col < cols; col++) {
                    int idx = start + row * cols + col;
                    if (idx < end) indices.push_back(idx);
                }
                if (indices.empty()) break;

                auto getJadwalLine = [&](int idx, int lineNum) -> string {
                    const Jadwal* j = list[idx];
                    switch(lineNum) {
                        case 0: return "ID      : " + j->getIdJadwal();
                        case 1: return "Kegiatan: " + j->getNamaKegiatan();
                        case 2: return "Mulai   : " + formatTime(j->getMulai());
                        case 3: return "Selesai : " + formatTime(j->getSelesai());
                        case 4: return "";
                        default: return "";
                    }
                };

                bool isLastRow = (start + (row + 1) * cols >= end);
                for (int lineNum = 0; lineNum < 5; lineNum++) {
                    if (lineNum == 4 && isLastRow) continue;
                    for (int col = 0; col < (int)indices.size(); col++) {
                        int idx = indices[col];
                        const Jadwal* j = list[idx];
                        string color = (j->getSelesai() < now) ? GRAY : RESET;
                        string line = getJadwalLine(idx, lineNum);

                        if ((int)line.size() > colWidth - 2)
                            line = line.substr(0, colWidth - 5) + "...";

                        ostringstream padded;
                        padded << left << setw(colWidth) << line;
                        cout << color << padded.str() << RESET;
                    }
                    cout << endl;
                }
            }
        }

        cout << string(termCols, '-') << endl;
        cout << GRAY << "[←/→/↑/↓] atau [N/P] Navigate  [Q] Quit" << RESET << endl;
        cout.flush();

        // langsung keluar kalau tidak ada data
        if (total == 0) break;

        termios orig;
        tcgetattr(STDIN_FILENO, &orig);
        enableRawMode(orig);
        int c = readKey();
        disableRawMode(orig);

        if (c == 'n' && page < totalPages - 1) page++;
        else if (c == 'p' && page > 0) page--;
        else if (c == 'q') break;
    }
    cout << endl;
}

// pagination buat hasil search jadwal lintas ruangan
// sama kayak browseJadwal tapi tiap entry nampilin nama ruangan juga
void browseHasilSearch(const string& title, const vector<pair<const Jadwal*, string>>& results) {
    int total = results.size();
    int page = 0;
    time_t now = time(nullptr);

    while (true) {
        system("clear");

        auto [termCols, termRows] = getTerminalSize();

        // responsive columns
        int cols;
        if (termCols < 80) cols = 1;
        else if (termCols < 160) cols = 2;
        else cols = 3;

        int colWidth = termCols / cols;

        // tiap entry = 6 baris (+ nama ruangan), sisain 3 buat header/footer
        int rowsPerPage = (termRows - 3) / 6;
        if (rowsPerPage < 1) rowsPerPage = 1;
        int PAGE_SIZE = rowsPerPage * cols;
        int totalPages = (total + PAGE_SIZE - 1) / PAGE_SIZE;

        cout << GREEN << title << " (Halaman " << page+1 << "/" << totalPages << ")" << RESET << endl;
        cout << string(termCols, '-') << endl;

        int start = page * PAGE_SIZE;
        int end = min(start + PAGE_SIZE, total);

        for (int row = 0; row < rowsPerPage; row++) {
            vector<int> indices;
            for (int col = 0; col < cols; col++) {
                int idx = start + row * cols + col;
                if (idx < end) indices.push_back(idx);
            }
            if (indices.empty()) break;

            auto getLine = [&](int idx, int lineNum) -> string {
                const Jadwal* j = results[idx].first;
                const string& namaRuangan = results[idx].second;
                switch(lineNum) {
                    case 0: return "Ruangan : " + namaRuangan;
                    case 1: return "ID      : " + j->getIdJadwal();
                    case 2: return "Kegiatan: " + j->getNamaKegiatan();
                    case 3: return "Mulai   : " + formatTime(j->getMulai());
                    case 4: return "Selesai : " + formatTime(j->getSelesai());
                    case 5: return "";
                    default: return "";
                }
            };

            bool isLastRow = (start + (row + 1) * cols >= end);
            for (int lineNum = 0; lineNum < 6; lineNum++) {
                if (lineNum == 5 && isLastRow) continue;
                for (int col = 0; col < (int)indices.size(); col++) {
                    int idx = indices[col];
                    const Jadwal* j = results[idx].first;
                    string color = (j->getSelesai() < now) ? GRAY : RESET;
                    string line = getLine(idx, lineNum);

                    if ((int)line.size() > colWidth - 2)
                        line = line.substr(0, colWidth - 5) + "...";

                    ostringstream padded;
                    padded << left << setw(colWidth) << line;
                    cout << color << padded.str() << RESET;
                }
                cout << endl;
            }
        }

        cout << string(termCols, '-') << endl;
        cout << GRAY << "[←/→/↑/↓] atau [N/P] Navigate  [Q] Quit" << RESET << endl;
        cout.flush();

        termios orig;
        tcgetattr(STDIN_FILENO, &orig);
        enableRawMode(orig);
        int c = readKey();
        disableRawMode(orig);

        if (c == 'n' && page < totalPages - 1) page++;
        else if (c == 'p' && page > 0) page--;
        else if (c == 'q') break;
    }
}

// pagination buat hasil search ruangan tersedia
// single line per entry, mirip browseRuangan tapi nerima vector hasil search
void browseHasilRuangan(const string& title, const vector<pair<string, string>>& results) {
    int total = results.size();
    int page = 0;

    while (true) {
        system("clear");

        auto [termCols, termRows] = getTerminalSize();

        int cols;
        if (termCols < 60) cols = 1;
        else if (termCols < 120) cols = 2;
        else cols = 3;

        int colWidth = termCols / cols;

        int PAGE_SIZE = (termRows - 5) * cols;
        int totalPages = (total + PAGE_SIZE - 1) / PAGE_SIZE;

        cout << GREEN << title << " (Halaman " << page+1 << "/" << totalPages << ")" << RESET << endl;
        cout << string(termCols, '-') << endl;

        int start = page * PAGE_SIZE;
        int end = min(start + PAGE_SIZE, total);

        for (int i = start; i < end; i++) {
            string entry = "ID: " + results[i].first + "  " + results[i].second;

            if ((int)entry.size() > colWidth - 2)
                entry = entry.substr(0, colWidth - 5) + "...";

            ostringstream padded;
            padded << left << setw(colWidth) << entry;
            cout << padded.str();
            if ((i - start + 1) % cols == 0) cout << endl;
        }

        if ((end - start) % cols != 0) cout << endl;

        cout << string(termCols, '-') << endl;
        cout << GRAY << "[←/→/↑/↓] atau [N/P] Navigate  [Q] Quit" << RESET << endl;
        cout.flush();

        termios orig;
        tcgetattr(STDIN_FILENO, &orig);
        enableRawMode(orig);
        int c = readKey();
        disableRawMode(orig);

        if (c == 'n' && page < totalPages - 1) page++;
        else if (c == 'p' && page > 0) page--;
        else if (c == 'q') break;
    }
}

#endif