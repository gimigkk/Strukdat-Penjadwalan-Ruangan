#ifndef JADWAL_H
#define JADWAL_H

#include <bits/stdc++.h>

using namespace std;

class Jadwal {
    private:
        time_t mulai;
        time_t selesai;
        string namaKegiatan;
        string id;

    public:
        Jadwal() : mulai(0), selesai(0), namaKegiatan(""), id("") {}
        Jadwal(time_t m, time_t s, string n, string i) : mulai(m), selesai(s), namaKegiatan(n), id(i) {}
        
        // ini buat keperluan cek jadwal overlap
        time_t getMulai() const { return mulai; }
        time_t getSelesai() const { return selesai; }
        string getId() const { return id; }

        void setId(const string& newId) {
            id = newId;
        }

        string getNamaKegiatan() const {
            return namaKegiatan;
        }

        string getIdJadwal() const {
            return id;
        }

        void setMulai(time_t m) {
            mulai = m;
        }

        void setSelesai(time_t s) {
            selesai = s;
        }

        void setNamaKegiatan(const string& nk) {
            namaKegiatan = nk;
        }

};

#endif