#ifndef STORAGE2_H //readwrite ke json
#define STORAGE2_H

#include <bits/stdc++.h>
#include "../nlohmann/json.hpp"
#include "../globals2.h"
#include "../latency.h"

using namespace std;
using json = nlohmann::json;

void Ruangan::tambahJadwalBaru(time_t mulai, time_t selesai, const string& namaKegiatan) {
    if (!cekKetersediaan(mulai, selesai)) {
        cout << RED << "Jadwal bentrok dengan jadwal lain." << RESET << endl;
        return;
    }
    string newId = generateJadwalId();
    Jadwal j(mulai, selesai, namaKegiatan, newId);

    ScopeTimer t("vector::push_back (tambahJadwal)");
    setJadwal(j);

    cout << CYAN << "Jadwal berhasil ditambahkan dengan ID " << newId << RESET << endl;
}

void Ruangan::ubahRuangan(const string& idRuanganLama, const string& idRuanganBaru, const string& idJadwal) {
    Ruangan* ruanganLama = findRuangan(idRuanganLama);
    Ruangan* ruanganBaru = findRuangan(idRuanganBaru);

    if (!ruanganLama || !ruanganBaru) {
        cout << RED << "Data tidak ditemukan" << RESET << endl;
        return;
    }

    const vector<Jadwal>& jadwalVec = ruanganLama->getJadwal();
    auto itJadwal = find_if(jadwalVec.begin(), jadwalVec.end(),
        [&](const Jadwal& j) { return j.getIdJadwal() == idJadwal; });

    if (itJadwal == jadwalVec.end()) {
        cout << RED << "Jadwal tidak ditemukan di ruangan lama." << RESET << endl;
        return;
    }

    const Jadwal& dataJadwal = *itJadwal;

    const vector<Jadwal>& jadwalBaru = ruanganBaru->getJadwal();
    bool sudahAda = any_of(jadwalBaru.begin(), jadwalBaru.end(),
        [&](const Jadwal& j) { return j.getIdJadwal() == idJadwal; });

    if (sudahAda) {
        cout << "ID jadwal sudah ada di ruangan baru. Proses pembaruan dibatalkan.\n";
        return;
    }

    if (!ruanganBaru->cekKetersediaan(dataJadwal.getMulai(), dataJadwal.getSelesai())) {
        cout << "Ruangan baru tidak tersedia.\n";
        return;
    }

    {
        ScopeTimer t("vector::push_back+erase (ubahRuangan)");
        ruanganBaru->setJadwal(dataJadwal);
        ruanganLama->hapusJadwalById(idJadwal);
    }

    cout << CYAN << "\nJadwal telah dipindah dari " << ruanganLama->getNamaRuangan()
         << " ke " << ruanganBaru->getNamaRuangan() << RESET << endl;
}

void readJson(const string& file) {
    json data;
    ifstream File(file);

    if (!File.is_open()) {
        cout << RED << "File not found" << RESET << endl;
        return;
    }

    {
        ScopeTimer t("json::parse (readJson)");
        File >> data;
        File.close();
    }

    daftarRuangan.clear();

    int maxJadwalNumber = 0;

    {
        ScopeTimer t("vector::push_back (readJson)");
        for (auto& ruangan : data["ruangan"]) {
            string id          = ruangan["id"];
            string namaRuangan = ruangan["namaRuangan"];

            Ruangan dataRuangan(namaRuangan, id);

            for (auto& jadwal : ruangan["jadwal"]) {
                string  idJ          = jadwal["id"];
                string  namaKegiatan = jadwal["namaKegiatan"];
                time_t  mulai        = jadwal["mulai"];
                time_t  selesai      = jadwal["selesai"];

                Jadwal dataJadwal(mulai, selesai, namaKegiatan, idJ);
                dataRuangan.loadJadwal(dataJadwal);

                maxJadwalNumber = max(maxJadwalNumber, extractJadwalNumber(idJ));
            }
            daftarRuangan.push_back(dataRuangan);
        }
    }

    globalJadwalCounter = maxJadwalNumber + 1;
}

void writeJson(const string& file) {
    json data;
    data["ruangan"] = json::array();

    {
        ScopeTimer t("vector::scan (writeJson)");
        for (auto& r : daftarRuangan) {
            json dr;
            dr["id"]          = r.getId();
            dr["namaRuangan"] = r.getNamaRuangan();
            dr["jadwal"]      = json::array();

            for (const auto& j : r.getJadwal()) {
                json dj;
                dj["id"]           = j.getIdJadwal();
                dj["namaKegiatan"] = j.getNamaKegiatan();
                dj["mulai"]        = j.getMulai();
                dj["selesai"]      = j.getSelesai();
                dr["jadwal"].push_back(dj);
            }
            data["ruangan"].push_back(dr);
        }
    }

    {
        ScopeTimer t("json::write (writeJson)");
        ofstream File(file, ios::trunc);
        File << data.dump(4);
    }
}

#endif