#ifndef MENU2_H
#define MENU2_H

#include <bits/stdc++.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include "../globals2.h"
#include "../latency.h"

using namespace std;

void enableRawMode(termios& orig) {
    termios raw = orig;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void disableRawMode(termios& orig) {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig);
}

int readKey() {
    char c;
    while (true) {
        if (read(STDIN_FILENO, &c, 1) <= 0) return 'q';
        if (c != '\n' && c != '\r') break;
    }

    if (c == '\033') {
        char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) <= 0) return 'q';
        if (read(STDIN_FILENO, &seq[1], 1) <= 0) return 'q';
        if (seq[0] == '[') {
            if (seq[1] == 'C') return 'n';
            if (seq[1] == 'D') return 'p';
            if (seq[1] == 'B') return 'n';
            if (seq[1] == 'A') return 'p';
        }
    }
    return tolower(c);
}

pair<int,int> getTerminalSize() {
    struct winsize w = {};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col == 0 || w.ws_row == 0) {
        return {120, 25};
    }
    return {w.ws_col, w.ws_row};
}

void browseRuangan() {
    vector<pair<string, string>> list;
    {
        ScopeTimer t("vector::scan (browseRuangan)");
        for (const auto& r : daftarRuangan)
            list.push_back({r.getId(), r.getNamaRuangan()});
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

        int cols;
        if (termCols < 60) cols = 1;
        else if (termCols < 120) cols = 2;
        else cols = 3;

        int colWidth  = termCols / cols;
        int PAGE_SIZE = (termRows - 5) * cols;
        int totalPages = (total + PAGE_SIZE - 1) / PAGE_SIZE;

        cout << GREEN << "Daftar Ruangan (Halaman " << page+1 << "/" << totalPages
             << ") [" << termCols << "x" << termRows << "]" << RESET << endl;
        cout << string(termCols, '-') << endl;

        int start = page * PAGE_SIZE;
        int end   = min(start + PAGE_SIZE, total);

        for (int i = start; i < end; i++) {
            string entry = "ID: " + list[i].first + "  " + list[i].second;
            if ((int)entry.size() > colWidth - 2)
                entry = entry.substr(0, colWidth - 5) + "...";
            cout << left << setw(colWidth) << entry;
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
        else if (c == 'p' && page > 0)         page--;
        else if (c == 'q')                      break;
    }
}

void browseJadwal(const Ruangan& ruangan) {
    vector<const Jadwal*> list;
    for (const auto& j : ruangan.getJadwal())
        list.push_back(&j);

    sort(list.begin(), list.end(), [](const Jadwal* a, const Jadwal* b) {
        return a->getMulai() > b->getMulai();
    });

    int total = list.size();
    int page  = 0;
    time_t now = time(nullptr);

    while (true) {
        system("clear");
        auto [termCols, termRows] = getTerminalSize();

        int cols;
        if (termCols < 80)       cols = 1;
        else if (termCols < 160) cols = 2;
        else                     cols = 3;

        int colWidth    = termCols / cols;
        int rowsPerPage = (termRows - 3) / 5;
        if (rowsPerPage < 1) rowsPerPage = 1;
        int PAGE_SIZE  = rowsPerPage * cols;
        int totalPages = (total + PAGE_SIZE - 1) / PAGE_SIZE;

        cout << GREEN << "Jadwal Ruangan: " << ruangan.getNamaRuangan()
             << " (Halaman " << page+1 << "/" << totalPages << ")" << RESET << endl;
        cout << string(termCols, '-') << endl;

        int start = page * PAGE_SIZE;
        int end   = min(start + PAGE_SIZE, total);

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
                    string line  = getJadwalLine(idx, lineNum);
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

        if (total == 0) break;

        termios orig;
        tcgetattr(STDIN_FILENO, &orig);
        enableRawMode(orig);
        int c = readKey();
        disableRawMode(orig);

        if (c == 'n' && page < totalPages - 1) page++;
        else if (c == 'p' && page > 0)         page--;
        else if (c == 'q')                      break;
    }
}

void browseJadwalByDate(const Ruangan& ruangan, time_t targetDate) {
    vector<const Jadwal*> list;
    tm* targetTm   = localtime(&targetDate);
    int targetYear = targetTm->tm_year;
    int targetMonth= targetTm->tm_mon;
    int targetDay  = targetTm->tm_mday;

    for (const auto& data : ruangan.getJadwal()) {
        time_t mulai = data.getMulai();
        tm* jadwalTm = localtime(&mulai);
        if (jadwalTm->tm_year == targetYear &&
            jadwalTm->tm_mon  == targetMonth &&
            jadwalTm->tm_mday == targetDay) {
            list.push_back(&data);
        }
    }

    sort(list.begin(), list.end(), [](const Jadwal* a, const Jadwal* b) {
        return a->getMulai() > b->getMulai();
    });

    int total = list.size();
    int page  = 0;
    time_t now = time(nullptr);

    while (true) {
        system("clear");
        auto [termCols, termRows] = getTerminalSize();

        int cols;
        if (termCols < 80)       cols = 1;
        else if (termCols < 160) cols = 2;
        else                     cols = 3;

        int colWidth    = termCols / cols;
        int rowsPerPage = (termRows - 3) / 5;
        if (rowsPerPage < 1) rowsPerPage = 1;
        int PAGE_SIZE  = rowsPerPage * cols;
        int totalPages = max(1, (total + PAGE_SIZE - 1) / PAGE_SIZE);

        cout << GREEN << "Jadwal Ruangan: " << ruangan.getNamaRuangan()
             << " pada " << formatDate(targetDate)
             << " (Halaman " << page+1 << "/" << totalPages << ")" << RESET << endl;
        cout << string(termCols, '-') << endl;

        if (total == 0) {
            cout << "Tidak ada jadwal pada tanggal " << formatDate(targetDate) << "." << RESET << endl;
        } else {
            int start = page * PAGE_SIZE;
            int end   = min(start + PAGE_SIZE, total);

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
                        string line  = getJadwalLine(idx, lineNum);
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

        if (total == 0) break;

        termios orig;
        tcgetattr(STDIN_FILENO, &orig);
        enableRawMode(orig);
        int c = readKey();
        disableRawMode(orig);

        if (c == 'n' && page < totalPages - 1) page++;
        else if (c == 'p' && page > 0)         page--;
        else if (c == 'q')                      break;
    }
    cout << endl;
}

void browseHasilSearch(const string& title, const vector<pair<const Jadwal*, string>>& results) {
    int total = results.size();
    int page  = 0;
    time_t now = time(nullptr);

    while (true) {
        system("clear");
        auto [termCols, termRows] = getTerminalSize();

        int cols;
        if (termCols < 80)       cols = 1;
        else if (termCols < 160) cols = 2;
        else                     cols = 3;

        int colWidth    = termCols / cols;
        int rowsPerPage = (termRows - 3) / 6;
        if (rowsPerPage < 1) rowsPerPage = 1;
        int PAGE_SIZE  = rowsPerPage * cols;
        int totalPages = (total + PAGE_SIZE - 1) / PAGE_SIZE;

        cout << GREEN << title << " (Halaman " << page+1 << "/" << totalPages << ")" << RESET << endl;
        cout << string(termCols, '-') << endl;

        int start = page * PAGE_SIZE;
        int end   = min(start + PAGE_SIZE, total);

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
                    string line  = getLine(idx, lineNum);
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
        else if (c == 'p' && page > 0)         page--;
        else if (c == 'q')                      break;
    }
}

void browseHasilRuangan(const string& title, const vector<pair<string, string>>& results) {
    int total = results.size();
    int page  = 0;

    while (true) {
        system("clear");
        auto [termCols, termRows] = getTerminalSize();

        int cols;
        if (termCols < 60)       cols = 1;
        else if (termCols < 120) cols = 2;
        else                     cols = 3;

        int colWidth  = termCols / cols;
        int PAGE_SIZE = (termRows - 5) * cols;
        int totalPages = (total + PAGE_SIZE - 1) / PAGE_SIZE;

        cout << GREEN << title << " (Halaman " << page+1 << "/" << totalPages << ")" << RESET << endl;
        cout << string(termCols, '-') << endl;

        int start = page * PAGE_SIZE;
        int end   = min(start + PAGE_SIZE, total);

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
        else if (c == 'p' && page > 0)         page--;
        else if (c == 'q')                      break;
    }
}

#endif
