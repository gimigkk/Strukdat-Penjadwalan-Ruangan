#include <bits/stdc++.h>
using namespace std;

class Jadwal {
    private:
        time_t mulai;
        time_t selesai;
        string namaKegiatan;
        string id;

    public:
        Jadwal(time_t m, time_t s, string n, string i) : mulai(m), selesai(s), namaKegiatan(n), id(i) {}
        
        // ini buat keperluan cek jadwal overlap
        time_t getMulai() const { return mulai; }
        time_t getSelesai() const { return selesai; }
};

class Ruangan {
    private:
        string namaRuangan;
        string id;
        vector<Jadwal> daftarJadwal;

    public:
        Ruangan(string n, string i) : namaRuangan(n), id(i) {}
        
        void tambahJadwal(const Jadwal& j) {
            daftarJadwal.push_back(j);
        }
        
        bool cekKetersediaan(time_t mulai, time_t selesai) {
            for (const auto& j : daftarJadwal) {
                // Check overlap: jika tidak -> (selesai sebelum mulai OR mulai setelah selesai) -> berarti ada overlap
                if (!(selesai <= j.getMulai() || mulai >= j.getSelesai())) {
                    return false;
                }
            }
            // berarti gada yang overlap
            return true;
        }
};