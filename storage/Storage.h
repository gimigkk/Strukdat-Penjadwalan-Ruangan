#ifndef STORAGE_H
#define STORAGE_H

#include <bits/stdc++.h>
#include "../nlohmann/json.hpp"
#include "../globals.h"

using namespace std;
using json = nlohmann::json;

void Ruangan::tambahJadwalBaru(time_t mulai, time_t selesai, const string& namaKegiatan) {
    if (!cekKetersediaan(mulai, selesai)) {
        cout << RED << "Jadwal bentrok dengan jadwal lain." << RESET << endl;
        return;
    }

    string newId = generateJadwalId();
    Jadwal j(mulai, selesai, namaKegiatan, newId);
    setJadwal(j);

    cout << CYAN << "Jadwal berhasil ditambahkan dengan ID " << newId << RESET << endl;
}

void Ruangan::ubahRuangan(const string& idRuanganLama, const string& idRuanganBaru, const string& idJadwal) {
    auto itJadwal = daftarJadwal.find(idJadwal);
    auto itRuanganLama = daftarRuangan.find(idRuanganLama);
    auto itRuanganBaru = daftarRuangan.find(idRuanganBaru);
    if(itJadwal == daftarJadwal.end() || itRuanganBaru == daftarRuangan.end() || itRuanganLama == daftarRuangan.end()) {
        cout << RED << "Data tidak ditemukan" << RESET << endl;
        return;
    }
    Jadwal& dataJadwal = itJadwal->second;
    Ruangan& ruanganLama = itRuanganLama->second;
    Ruangan& ruanganBaru = itRuanganBaru->second;

    if (ruanganBaru.getJadwal().find(idJadwal) != ruanganBaru.getJadwal().end()) {
        cout << "ID jadwal sudah ada di ruangan baru. Proses pembaruan dibatalkan.\n";
        return;
    }

    if (!ruanganBaru.cekKetersediaan(dataJadwal.getMulai(), dataJadwal.getSelesai())) {
        cout << "Ruangan baru tidak tersedia.\n";
        return;
    }  

    // Tambah jadwal di ruangan baru
    ruanganBaru.setJadwal(dataJadwal);
    // Hapus jadwal di ruangan lama
    ruanganLama.hapusJadwalById(idJadwal);

    cout << CYAN << "\nJadwal telah dipindah dari " << ruanganLama.getNamaRuangan() << " ke " << ruanganBaru.getNamaRuangan() << RESET << endl;
}

// Fungsi buat read JSON
void readJson(const string& file) {
    json data;
    ifstream File(file);

    if(!File.is_open()) {
        cout << RED << "File not found" << endl;
        return;
    } 
    // Parse 
    File >> data;
    File.close();

    // Bersihin runtime storage
    daftarRuangan.clear();

    int maxJadwalNumber = 0;

    // Masukkin ke map
    for(auto& ruangan : data["ruangan"]) {
        string id = ruangan["id"];
        string namaRuangan = ruangan["namaRuangan"];

        Ruangan dataRuangan(namaRuangan, id);

        for(auto& jadwal : ruangan["jadwal"]) {
            string id = jadwal["id"];
            string namaKegiatan = jadwal["namaKegiatan"];
            time_t mulai = jadwal["mulai"];
            time_t selesai = jadwal["selesai"];

            Jadwal dataJadwal(mulai, selesai, namaKegiatan, id);
            dataRuangan.loadJadwal(dataJadwal);

            maxJadwalNumber = max(maxJadwalNumber, extractJadwalNumber(id));
        }
        daftarRuangan.insert({dataRuangan.getId(), dataRuangan});
    }
    globalJadwalCounter = maxJadwalNumber + 1;
}

// Buat masukkin data ke json
// map udah sorted by key, jadi urutan di json selalu konsisten
void writeJson(const string& file) {
    json data;

    // Bikin array of ruangan
    data["ruangan"] = json::array();
    for(auto& r : daftarRuangan) {
        Ruangan& ruangan = r.second;
        json dr; // data ruangan

        dr["id"] = ruangan.getId();
        dr["namaRuangan"] = ruangan.getNamaRuangan();

        // bikin array of jadwal
        dr["jadwal"] = json::array();

        for(auto& j: ruangan.getJadwal()) {
            const Jadwal& jadwal = j.second;
            
            json dj; // data jadwal

            dj["id"] = jadwal.getIdJadwal();
            dj["namaKegiatan"] = jadwal.getNamaKegiatan();
            dj["mulai"] = jadwal.getMulai();
            dj["selesai"] = jadwal.getSelesai();

            dr["jadwal"].push_back(dj);
        }
        data["ruangan"].push_back(dr);
    }
    // Write dengan ditruncate dlu 
    ofstream File(file, ios::trunc); 
    File << data.dump(4);
}

#endif