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

        string getNamaKegiatan() const {
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
        
        bool cekKetersediaan(time_t mulai, time_t selesai) const {
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

        void printJadwal() const {
            if(daftarJadwal.empty()) {
                cout << "Tidak ada jadwal untuk ruangan ini." << endl;
                return;
            }

            cout << "Jadwal Ruangan: "<< getNamaRuangan() << endl;
            cout << "---" << endl;
            for (const auto& it : daftarJadwal) {
                cout << "Kegiatan\t: " << it.getNamaKegiatan() << endl;
                cout << "Jam Mulai\t: " << formatTime(it.getMulai()) << endl;
                cout << "Jam Selesai\t: " << formatTime(it.getSelesai()) << endl;
                cout << "---" << endl;
            }
            return;
        }
};

// runtime storage kita
// buat presistent storage kita simpen di file
// tapi pikirin lagi karena gw ga kepikiran.
unordered_map<string, Ruangan> daftarRuangan;

// Print semua jadwal dari ruangan tertentu
void searchJadwalRuangan(const unordered_map<string, Ruangan>& daftarRuangan) {
    string searchId;
    cout << "Masukkan ID Ruangan yang ingin dicari: ";
    cin >> searchId;
    cout << endl;

    auto it = daftarRuangan.find(searchId);
    if (it != daftarRuangan.end()) {
        it->second.printJadwal();
    } else {
        cout << "Ruangan dengan ID " << searchId << " tidak ditemukan." << endl;
    }
}

// Cari ruangan yang tersedia pada waktu tertentu
void searchRuanganTersedia(const unordered_map<string, Ruangan>& daftarRuangan, time_t mulai, time_t selesai) {
    cout << "List ruangan tersedia dari " << formatHourMinute(mulai) << " sampai " << formatHourMinute(selesai) << ",\npada tanggal " << formatDate(mulai) << ":" << endl;
    cout << "---" << endl;
    bool found = false;
    for (const auto& pair : daftarRuangan) {
        if (pair.second.cekKetersediaan(mulai, selesai)) {
            cout << "ID: " << pair.second.getId() << ", Nama: " << pair.second.getNamaRuangan() << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "Tidak ada." << endl;
    }
    cout << "---" << endl;
}

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
            cout << RED << "ERR: Jadwal overlap." << RESET << endl;
        }
    } 
    else {
        cout << RED << "ERR: Ruangan tidak ditemukan." << RESET << endl;
    }

    // ngerti ga?

    /// Program Loop
    while (true) {
        cout << "\n" << GREEN << "Menu:" << RESET << endl;
        cout << GREEN << "1. Lihat List Ruangan" << RESET  << endl;
        cout << GREEN << "2. Search Jadwal dari Ruangan" << RESET  << endl;
        cout << GREEN << "3. Search Jadwal dari Waktu" << RESET << endl;

        int choice;
        cout << GREEN << "Pilih menu (1-3): " << RESET;
        if (!(cin >> choice)) {
            cout << endl;
            cout << RED << "ERR: Input harus angka." << RESET << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cout << endl;

        switch(choice) {
            case 1: {
                cout << "Daftar Ruangan:" << endl;
                for (const auto& pair : daftarRuangan) {
                    cout << "ID: " << pair.second.getId() << ", Nama: " << pair.second.getNamaRuangan() << endl;
                }
                break;
            }
            case 2: {
                searchJadwalRuangan(daftarRuangan);
                break;
            }
            case 3: {
                int thn, bln, hari, jamMulai, menitMulai, jamSelesai, menitSelesai;
                cout << "Tanggal (YYYY MM DD)\t: ";
                cin >> thn >> bln >> hari;

                cout << "Waktu mulai (HH MM)\t: ";
                cin >> jamMulai >> menitMulai;
                time_t mulai = makeTime(thn, bln, hari, jamMulai, menitMulai);

                cout << "Waktu selesai (HH MM)\t: ";
                cin >> jamSelesai >> menitSelesai;
                time_t selesai = makeTime(thn, bln, hari, jamSelesai, menitSelesai);

                cout << endl;

                searchRuanganTersedia(daftarRuangan, mulai, selesai);
                break;
            }
            default:
                cout << "Pilihan tidak valid. Silakan coba lagi." << endl;
                break;
        }

    }

    return 0;
}