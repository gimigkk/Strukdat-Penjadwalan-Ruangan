#ifndef GLOBALS_H
#define GLOBALS_H

#include <bits/stdc++.h>
#include "models/Ruangan.h"

using namespace std;

// runtime storage kita
// buat presistent storage kita simpen di file
// tapi pikirin lagi karena gw ga kepikiran.
map<string, Ruangan> daftarRuangan;

// Auto generate ID
int globalJadwalCounter = 1;

string generateJadwalId() {
    stringstream ss;
    ss << "J" << setw(6) << setfill('0') << globalJadwalCounter++;
    return ss.str();
}

int extractJadwalNumber(const string& id) {
    if (id.size() < 2 || id[0] != 'J') return 0;
    return stoi(id.substr(1));
}

bool isJadwalIdExistGlobal(const string& idJadwal) {
    for (const auto& pair : daftarRuangan) {
        const auto& jadwalMap = pair.second.getJadwal();
        if (jadwalMap.find(idJadwal) != jadwalMap.end()) {
            return true;
        }
    }
    return false;
}

string makeUniqueJadwalIdGlobal(const string& baseId) {
    if (!isJadwalIdExistGlobal(baseId)) return baseId;

    for (int i = 2; ; i++) {
        string candidate = baseId + "_" + to_string(i);
        if (!isJadwalIdExistGlobal(candidate)) {
            return candidate;
        }
    }
}

#endif