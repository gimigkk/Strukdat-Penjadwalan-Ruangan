#ifndef RUANGAN2_H
#define RUANGAN2_H

#include <bits/stdc++.h>
#include "Jadwal.h"
#include "../utils.h"

using namespace std;

class Ruangan {
    private:
        string namaRuangan;
        string id;
        vector<Jadwal> daftarJadwal;  

    public:
        Ruangan() : namaRuangan(" "), id(" ") {}
        Ruangan(string n, string i) : namaRuangan(n), id(i) {}

        string getId() const { return id; }
        string getNamaRuangan() const { return namaRuangan; }

        const vector<Jadwal>& getJadwal() const {
            return daftarJadwal;
        }

        void setJadwal(const Jadwal& j) {
            for (auto& existing : daftarJadwal) {
                if (existing.getIdJadwal() == j.getIdJadwal()) {
                    existing = j;
                    return;
                }
            }
            daftarJadwal.push_back(j);
        }

        bool hapusJadwalById(const string& idJadwal) {
            auto it = find_if(daftarJadwal.begin(), daftarJadwal.end(),
                [&](const Jadwal& j) { return j.getIdJadwal() == idJadwal; });
            if (it != daftarJadwal.end()) {
                daftarJadwal.erase(it);
                return true;
            }
            return false;
        }

        bool cekKetersediaan(time_t mulai, time_t selesai, const string& ignoreIdJadwal = "") const {
            for (const auto& data : daftarJadwal) {
                if (data.getIdJadwal() == ignoreIdJadwal) continue;
                if (!(selesai <= data.getMulai() || mulai >= data.getSelesai())) {
                    return false;
                }
            }
            return true;
        }

        void printJadwal() const {
            cout << "Jadwal Ruangan: " << getNamaRuangan() << RESET << endl;
            cout << "---" << RESET << endl;
            if (daftarJadwal.empty()) {
                cout << RED << "Tidak ada jadwal untuk ruangan ini." << RESET << endl;
                return;
            }
            for (const auto& data : daftarJadwal) {
                cout << "ID Jadwal: "    << data.getIdJadwal()     << RESET << endl;
                cout << "Kegiatan\t: "   << data.getNamaKegiatan() << RESET << endl;
                cout << "Jam Mulai\t: "  << formatTime(data.getMulai())    << RESET << endl;
                cout << "Jam Selesai\t: "<< formatTime(data.getSelesai())  << RESET << endl;
                cout << "---" << RESET << endl;
            }
        }

        void ubahWaktu(const string& idJadwal, time_t m, time_t s) {
            auto it = find_if(daftarJadwal.begin(), daftarJadwal.end(),
                [&](const Jadwal& j) { return j.getIdJadwal() == idJadwal; });

            if (it == daftarJadwal.end()) {
                cout << RED << "Data tidak ditemukan" << RESET << endl;
                return;
            }
            if (!cekKetersediaan(m, s, idJadwal)) {
                cout << RED << "\nMaaf, ruangan sudah terpakai." << RESET << endl;
                return;
            }
            it->setMulai(m);
            it->setSelesai(s);
            cout << CYAN << "Waktu untuk jadwal dengan ID " << it->getIdJadwal()
                 << " berhasil diubah" << RESET << endl;
        }

        void ubahNamaKegiatan(const string& idJadwal, const string& nk) {
            auto it = find_if(daftarJadwal.begin(), daftarJadwal.end(),
                [&](const Jadwal& j) { return j.getIdJadwal() == idJadwal; });

            if (it == daftarJadwal.end()) {
                cout << RED << "Data tidak ditemukan" << RESET << endl;
                return;
            }
            string currentNamaKegiatan = it->getNamaKegiatan();
            it->setNamaKegiatan(nk);
            cout << CYAN << "Kegiatan \"" << currentNamaKegiatan
                 << "\" di ruangan " << getNamaRuangan()
                 << " berhasil diubah menjadi \"" << nk << "\"." << RESET << endl;
        }

        void printAllJadwal() const {
            cout << GREEN << "Jadwal Ruangan: " << getNamaRuangan() << RESET << endl;
            for (const auto& data : daftarJadwal) {
                cout << "ID Jadwal: "    << data.getIdJadwal()     << RESET << endl;
                cout << "Kegiatan\t: "   << data.getNamaKegiatan() << RESET << endl;
                cout << "Jam Mulai\t: "  << formatTime(data.getMulai())   << RESET << endl;
                cout << "Jam Selesai\t: "<< formatTime(data.getSelesai()) << RESET << endl;
                cout << "---" << RESET << endl;
            }
        }

        void tambahJadwalBaru(time_t mulai, time_t selesai, const string& namaKegiatan);
        void ubahRuangan(const string& idRuanganLama, const string& idRuanganBaru, const string& idJadwal);
        void loadJadwal(const Jadwal& j);
        void printJadwalByDate(time_t targetDate) const;
};

void Ruangan::loadJadwal(const Jadwal& j) {
    daftarJadwal.push_back(j);
}

void Ruangan::printJadwalByDate(time_t targetDate) const {
    tm* targetTm = localtime(&targetDate);
    int targetYear  = targetTm->tm_year;
    int targetMonth = targetTm->tm_mon;
    int targetDay   = targetTm->tm_mday;

    cout << GREEN << "Jadwal Ruangan: " << getNamaRuangan()
         << " pada tanggal " << formatDate(targetDate) << RESET << endl;
    cout << "---" << RESET << endl;

    bool found = false;
    for (const auto& data : daftarJadwal) {
        time_t waktuMulai = data.getMulai();
        tm* jadwalTm = localtime(&waktuMulai);
        if (jadwalTm->tm_year == targetYear &&
            jadwalTm->tm_mon  == targetMonth &&
            jadwalTm->tm_mday == targetDay) {
            cout << "ID Jadwal: "    << data.getIdJadwal()     << RESET << endl;
            cout << "Kegiatan\t: "   << data.getNamaKegiatan() << RESET << endl;
            cout << "Jam Mulai\t: "  << formatTime(data.getMulai())   << RESET << endl;
            cout << "Jam Selesai\t: "<< formatTime(data.getSelesai()) << RESET << endl;
            cout << "---" << RESET << endl;
            found = true;
        }
    }
    if (!found) {
        cout << RED << "Tidak ada jadwal pada tanggal "
             << formatDate(targetDate) << "." << RESET << endl;
    }
}

#endif