// note untuk struktur datanya:
// Ruangan punya daftar jadwal
// Jadwal punya waktu mulai, waktu selesai, nama kegiatan, id
// Ruangan di simpen pake hashmap biar gampang aksesnya O(1)
// Key di hashmap itu id ruangan.

#include <bits/stdc++.h>
#include "utils.h" 
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

        string getNamaKegiatan() {
            return namaKegiatan;
        }

};

class Ruangan {
    private:
        string namaRuangan;
        string id;
        vector<Jadwal> daftarJadwal;

    public:
        Ruangan() : namaRuangan(" "), id(" ") {}
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

        string getId() const { return id; }
        string getNamaRuangan() const { return namaRuangan; }

        void printJadwal() {
            cout << "Ruangan: "<< getNamaRuangan() << endl;
            cout << "Jadwal" << endl;
            cout << "=========================" << endl;
            for (auto& it : daftarJadwal) {
                cout << "Kegiatan: " << it.getNamaKegiatan() << endl;
                cout << "Jam Mulai: " << formatTime(it.getMulai()) << "\nJam Selesai: " << formatTime(it.getSelesai()) << endl;
                cout << "-------------------------" << endl;
            }
        }
};

// runtime storage kita
// buat presistent storage kita simpen di file
// tapi pikirin lagi karena gw ga kepikiran.
unordered_map<string, Ruangan> daftarRuangan;

int main (){
    // Fake data buat mempermudah testing
    Ruangan r1("Ruang A", "R001");
    Ruangan r2("Ruang B", "R002");

    daftarRuangan.insert({{r1.getId(), r1}, {r2.getId(), r2}});

    // contoh implementasi bikin jadwal di suatu ruangan
    time_t mulai = makeTime(2026, 2, 14, 9, 0);
    time_t selesai = makeTime(2026, 2, 14, 10, 0);

    // bikin jadwal pake constructor.
    Jadwal j1(mulai, selesai, "Meeting Tim", "J001"); 

    // cek ketersediaan ruangan pake id ruangan sebagai key map.
    // inset juga pake id ruangan.

    // Cek dulu pakai find()
    if (daftarRuangan.find("R002") != daftarRuangan.end()) {
        // Kalau ada, baru pakai []
        if (daftarRuangan["R002"].cekKetersediaan(mulai, selesai)) {
            daftarRuangan["R002"].tambahJadwal(j1);
            cout << "Jadwal berhasil ditambahkan." << endl;
        } 
        else {
            cout << "ERR: Jadwal overlap." << endl;
        }
    } 
    else {
        cout << "ERR: Ruangan tidak ditemukan." << endl;
    }

    // ngerti ga?
    return 0;
}