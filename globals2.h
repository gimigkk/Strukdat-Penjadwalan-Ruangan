#ifndef GLOBALS_H
#define GLOBALS_H

#include <bits/stdc++.h>
#include "models/Ruangan2.h"

using namespace std;

// runtime storage kita
// buat presistent storage kita simpen di file
// tapi pikirin lagi karena gw ga kepikiran.
vector<Ruangan> daftarRuangan;

// Auto generate ID
int globalJadwalCounter = 1;

string generateJadwalId() {
    stringstream ss;
    ss << "J" << setw(4) << setfill('0') << globalJadwalCounter++;
    return ss.str();
}

int extractJadwalNumber(const string& id) {
    if (id.size() < 2 || id[0] != 'J') return 0;
    return stoi(id.substr(1));
}

//logika Vector
bool isJadwalIdExistGlobal(const string& idJadwal) {
    for (const auto& ruangan : daftarRuangan) {
        const auto& jadwalVector = ruangan.getJadwal();
        for (const auto& j : jadwalVector) {
            if (j.getId() == idJadwal) return true;
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

//cari ruangan pake ID kalau gaada return nullptr
inline Ruangan* findRuangan(const string& id) {
  for(auto& r : daftarRuangan)
    if (r.getId() == id) return &r;
  return nullptr;
}

inline const Ruangan* findRuanganConst(const string& id) {
  for(const auto& r : daftarRuangan)
    if (r.getId() == id) return &r;
  return nullptr;
}

#endif
