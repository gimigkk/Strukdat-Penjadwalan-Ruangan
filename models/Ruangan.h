#ifndef RUANGAN_H
#define RUANGAN_H

#include <bits/stdc++.h>
#include "Jadwal.h"
#include "../utils.h"

using namespace std;

class Ruangan {
    private:
        string namaRuangan;
        string id;
        map<string, Jadwal> daftarJadwal;

    public:
        Ruangan() : namaRuangan(" "), id(" ") {}
        Ruangan(string n, string i) : namaRuangan(n), id(i) {}

        string getId() const { return id; }
        string getNamaRuangan() const { return namaRuangan; }

        const auto& getJadwal() const{
            return daftarJadwal;
        }
        
        void setJadwal(const Jadwal& j) {
            daftarJadwal[j.getIdJadwal()] = j;
        }

        bool hapusJadwalById(const string& idJadwal) {
            return daftarJadwal.erase(idJadwal) > 0;
        }
        
        bool cekKetersediaan(time_t mulai, time_t selesai, const string& ignoreIdJadwal = "") const {
            for (const auto& j : daftarJadwal) {
                const string& idJadwal = j.first;
                const Jadwal& data = j.second;
            
                if (idJadwal == ignoreIdJadwal) continue;
            
                if (!(selesai <= data.getMulai() || mulai >= data.getSelesai())) {
                    return false;
                }
            }
            return true;
        }

        void printJadwal() const{
            cout << "Jadwal Ruangan: "<< getNamaRuangan() <<RESET<< endl;
            cout << "---" <<RESET<< endl;

            if(daftarJadwal.empty()) {
                cout << RED << "Tidak ada jadwal untuk ruangan ini." <<RESET<< endl;
                return;
            }

            for (const auto& it : daftarJadwal) {
                const Jadwal& data = it.second;
                cout << "ID Jadwal: "<< data.getIdJadwal() <<RESET<< endl;
                cout << "Kegiatan\t: " << data.getNamaKegiatan() <<RESET<< endl;
                cout << "Jam Mulai\t: " << formatTime(data.getMulai()) <<RESET<< endl;
                cout << "Jam Selesai\t: " << formatTime(data.getSelesai()) <<RESET<< endl;
                cout << "---" <<RESET<< endl;
            }
            return;
        }

        // Update waktu sebuah jadwal
        void ubahWaktu(const string& idJadwal, time_t m, time_t s) {

            auto it = daftarJadwal.find(idJadwal);
            if(it == daftarJadwal.end()) {
                cout << RED << "Data tidak ditemukan" << RESET << endl;
                return;
            }

            if(!cekKetersediaan(m, s, idJadwal)) {
                cout << RED << "\nMaaf, ruangan sudah terpakai." << RESET << endl;
                return;
            }

            Jadwal& data = it->second;
            data.setMulai(m);
            data.setSelesai(s);
            cout << CYAN << "Waktu untuk jadwal dengan ID " << data.getIdJadwal() << " berhasil diubah" << RESET << endl;
            return;
        }

        // Update nama kegiatan sebuah jadwal
        void ubahNamaKegiatan(const string& idJadwal, const string& nk) {
            auto it = daftarJadwal.find(idJadwal);
            if(it == daftarJadwal.end()) {
                cout << RED << "Data tidak ditemukan" << RESET << endl;
                return;
            }

            Jadwal& data = it->second;
            data.setNamaKegiatan(nk);
            cout << CYAN << "\nNama kegiatan untuk jadwal dengan ID " << data.getIdJadwal() << " berhasil diubah" << RESET << endl;
            return;
            
        }

        void printAllJadwal() const {
            cout << GREEN << "Jadwal Ruangan: "<< getNamaRuangan() <<RESET<< endl;
            for(const auto& pair : daftarJadwal) {
                const Jadwal& data = pair.second;
                cout << "ID Jadwal: "<< data.getIdJadwal() <<RESET<< endl;
                cout << "Kegiatan\t: " << data.getNamaKegiatan() <<RESET<< endl;
                cout << "Jam Mulai\t: " << formatTime(data.getMulai()) <<RESET<< endl;
                cout << "Jam Selesai\t: " << formatTime(data.getSelesai()) <<RESET<< endl;
                cout << "---" <<RESET<< endl;
            }
        }

        // Declarations for out-of-class implementations that need globals
        // (defined in storage/Storage.h)
        void tambahJadwalBaru(time_t mulai, time_t selesai, const string& namaKegiatan);
        void ubahRuangan(const string& idRuanganLama, const string& idRuanganBaru, const string& idJadwal);        
        void loadJadwal(const Jadwal& j);
        void printJadwalByDate(time_t targetDate) const;

};

// Calvin:
// Persistent storagenya pakai data.json 
// Di sini, rencananya : load at start, save at exit
// Perubahan data disimpen di hash map dulu
// Nanti data.json nya tinggal di overwrite pake data terbaru dari hash map

void Ruangan::loadJadwal(const Jadwal& j) {
    daftarJadwal[j.getIdJadwal()] = j;
}

void Ruangan::printJadwalByDate(time_t targetDate) const {
    tm* targetTm = localtime(&targetDate);

    int targetYear = targetTm->tm_year;
    int targetMonth = targetTm->tm_mon;
    int targetDay = targetTm->tm_mday;

    cout << GREEN << "Jadwal Ruangan: " << getNamaRuangan()
         << " pada tanggal " << formatDate(targetDate) << RESET << endl;
    cout << "---" << RESET << endl;

    bool found = false;

    for (const auto& it : daftarJadwal) {
        const Jadwal& data = it.second;

        time_t waktuMulai = data.getMulai();
        tm* jadwalTm = localtime(&waktuMulai);

        if (jadwalTm->tm_year == targetYear &&
            jadwalTm->tm_mon == targetMonth &&
            jadwalTm->tm_mday == targetDay) {

            cout << "ID Jadwal: " << data.getIdJadwal() << RESET << endl;
            cout << "Kegiatan\t: " << data.getNamaKegiatan() << RESET << endl;
            cout << "Jam Mulai\t: " << formatTime(data.getMulai()) << RESET << endl;
            cout << "Jam Selesai\t: " << formatTime(data.getSelesai()) << RESET << endl;
            cout << "---" << RESET << endl;

            found = true;
        }
    }

    if (!found) {
        cout << RED << "Tidak ada jadwal pada tanggal " << formatDate(targetDate) << "." << RESET << endl;
    }
}

#endif