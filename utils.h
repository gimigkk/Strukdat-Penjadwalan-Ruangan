#ifndef UTILS_H
#define UTILS_H

#include <ctime>
#include <string>
using namespace std;

// ANSI Color Codes
#define RESET "\033[0m"
#define CYAN "\033[36m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define GRAY "\033[90m"

// Membuat time_t dari input tahun, bulan, hari, jam, menit
inline time_t makeTime(int year, int month, int day, int hour, int minute) {
    tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_isdst = -1;
    return mktime(&t);
}

// Format time_t ke string "YYYY-MM-DD HH:MM"
inline string formatTime(time_t tt) {
    tm* t = localtime(&tt);
    char buffer[20];
    strftime(buffer, 20, "%Y-%m-%d %H:%M", t);
    return string(buffer);
}

inline string formatDate(time_t tt) {
    tm* t = localtime(&tt);
    char buffer[11];
    strftime(buffer, 11, "%Y-%m-%d", t);
    return string(buffer);
}

inline string formatHourMinute(time_t tt) {
    tm* t = localtime(&tt);
    char buffer[6];
    strftime(buffer, 6, "%H:%M", t);
    return string(buffer);
}

// Cek apakah dua time_t berada pada tanggal yang sama
inline bool sameDate(time_t a, time_t b) {
    tm* ta = localtime(&a);
    tm* tb = localtime(&b);
    return ta->tm_year == tb->tm_year &&
           ta->tm_mon == tb->tm_mon &&
           ta->tm_mday == tb->tm_mday;
}

// Helper buat input tanggal, support shortcut [T] untuk hari ini
// return tuple<int, int, int> = {thn, bln, hari}
inline tuple<int,int,int> inputTanggal() {
    while (true) {
        string input;
        cout << ">> Tanggal (YYYY MM DD) atau [T] untuk hari ini: ";

        if (!(cin >> input)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "Input tidak valid." << RESET << endl;
            continue;
        }

        if (input == "T" || input == "t") {
            time_t now = time(nullptr);
            tm* t = localtime(&now);
            return {t->tm_year + 1900, t->tm_mon + 1, t->tm_mday};
        }

        int thn, bln, hari;
        stringstream ss(input);
        if (!(ss >> thn) || !ss.eof()) {
            cout << RED << "Format tanggal salah. Gunakan YYYY MM DD atau T." << RESET << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (!(cin >> bln >> hari)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "Format tanggal salah. Gunakan YYYY MM DD atau T." << RESET << endl;
            continue;
        }

        tm t = {};
        t.tm_year = thn - 1900;
        t.tm_mon = bln - 1;
        t.tm_mday = hari;
        t.tm_hour = 0;
        t.tm_min = 0;
        t.tm_sec = 0;
        t.tm_isdst = -1;

        time_t parsed = mktime(&t);
        if (parsed == -1) {
            cout << RED << "Tanggal tidak valid." << RESET << endl;
            continue;
        }

        tm* check = localtime(&parsed);
        if (check->tm_year != t.tm_year || check->tm_mon != t.tm_mon || check->tm_mday != t.tm_mday) {
            cout << RED << "Tanggal tidak valid." << RESET << endl;
            continue;
        }

        return {thn, bln, hari};
    }
}

#endif