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

// Cek apakah dua time_t berada pada tanggal yang sama
inline bool sameDate(time_t a, time_t b) {
    tm* ta = localtime(&a);
    tm* tb = localtime(&b);
    return ta->tm_year == tb->tm_year &&
           ta->tm_mon == tb->tm_mon &&
           ta->tm_mday == tb->tm_mday;
}

#endif