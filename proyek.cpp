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
        Jadwal() : mulai(0), selesai(0), namaKegiatan(""), id("") {}
        Jadwal(time_t m, time_t s, string n, string i) : mulai(m), selesai(s), namaKegiatan(n), id(i) {}
        
        // ini buat keperluan cek jadwal overlap
        time_t getMulai() const { return mulai; }
        time_t getSelesai() const { return selesai; }
        string getId() const { return id; }

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

        void setNamaKegiatan(string nk) {
            namaKegiatan = nk;
        }

};

class Ruangan {
    private:
        string namaRuangan;
        string id;
        unordered_map<string, Jadwal> daftarJadwal;

    public:
        Ruangan() : namaRuangan(" "), id(" ") {}
        Ruangan(string n, string i) : namaRuangan(n), id(i) {}

        string getId() const { return id; }
        string getNamaRuangan() const { return namaRuangan; }
        unordered_map<string, Jadwal> getJadwal() {
            return daftarJadwal;
        }
        
        void tambahJadwal(const Jadwal& j) {
            daftarJadwal.insert({j.getIdJadwal(), j});
        }

        void hapusJadwalById(string idJadwal) {
            daftarJadwal.erase(idJadwal);
        }
        
        bool cekKetersediaan(time_t mulai, time_t selesai) const {
            for (const auto& j : daftarJadwal) {
                Jadwal data = j.second;
                // Check overlap: jika tidak -> (selesai sebelum mulai OR mulai setelah selesai) -> berarti ada overlap
                if (!(selesai <= data.getMulai() || mulai >= data.getSelesai())) {
                    return false;
                }
            }
            // berarti gada yang overlap
            return true;
        }

        void printJadwal() const {
            cout << "Jadwal Ruangan: "<< getNamaRuangan() <<RESET<< endl;
            cout << "---" <<RESET<< endl;

            if(daftarJadwal.empty()) {
                cout << RED << "Tidak ada jadwal untuk ruangan ini." <<RESET<< endl;
                return;
            }

            for (const auto& it : daftarJadwal) {
                Jadwal data = it.second;
                cout << "ID Jadwal: "<< data.getIdJadwal() <<RESET<< endl;
                cout << "Kegiatan\t: " << data.getNamaKegiatan() <<RESET<< endl;
                cout << "Jam Mulai\t: " << formatTime(data.getMulai()) <<RESET<< endl;
                cout << "Jam Selesai\t: " << formatTime(data.getSelesai()) <<RESET<< endl;
                cout << "---" <<RESET<< endl;
            }
            return;
        }

        // Update waktu sebuah jadwal
        void ubahWaktu(string idJadwal, time_t m, time_t s) {
            if(!cekKetersediaan(m, s)) {
                cout << "\nMaaf, jadwal tidak tersedia." << endl;
                return;
            }
            

            auto it = daftarJadwal.find(idJadwal);
            if(it == daftarJadwal.end()) {
                cout << RED << "Data tidak ditemukan" << RESET << endl;
                return;
            }

            Jadwal& data = it->second;
            data.setMulai(m);
            data.setSelesai(s);
            cout << CYAN << "Waktu untuk jadwal dengan ID " << data.getIdJadwal() << " berhasil diubah" << RESET << endl;
            return;
        }

        // Update nama kegiatan sebuah jadwal
        void ubahNamaKegiatan(string idJadwal, string nk) {
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

        // Update ruangan
        void ubahRuangan(string idRuanganLama, string idRuanganBaru, string idJadwal); 

};

// runtime storage kita
// buat presistent storage kita simpen di file
// tapi pikirin lagi karena gw ga kepikiran.
unordered_map<string, Ruangan> daftarRuangan;

// Print semua ruangan
void printAllRuangan() {
    cout << "Daftar Ruangan:" << endl;
    for (const auto& pair : daftarRuangan) {
        cout << "ID: " << pair.second.getId() << ", Nama: " << pair.second.getNamaRuangan() << endl;
    }
}

// Print semua jadwal dari ruangan tertentu

// Ganti tipe fungsi ke string
string searchJadwalRuangan(const unordered_map<string, Ruangan>& daftarRuangan) { 
    string searchId;
    cout << ">> Masukkan ID Ruangan yang ingin dicari: ";
    cin >> searchId;
    cout <<RESET<< endl;

    auto it = daftarRuangan.find(searchId);
    if (it != daftarRuangan.end()) {
        it->second.printJadwal();
        return searchId;
    } else {
        cout << "Ruangan dengan ID " << searchId << " tidak ditemukan." <<RESET<< endl;
        return " ";
    }
}

// Cari ruangan yang tersedia pada waktu tertentu
void searchRuanganTersedia(const unordered_map<string, Ruangan>& daftarRuangan) {
    int thn, bln, hari, jamMulai, menitMulai, jamSelesai, menitSelesai;
    cout << ">> Tanggal (YYYY MM DD)\t: ";
    cin >> thn >> bln >> hari;

    cout << ">> Waktu mulai (HH MM)\t: ";
    cin >> jamMulai >> menitMulai;
    time_t mulai = makeTime(thn, bln, hari, jamMulai, menitMulai);

    cout << left << setw(26)<< ">> Waktu selesai (HH MM): ";
    cin >> jamSelesai >> menitSelesai;
    time_t selesai = makeTime(thn, bln, hari, jamSelesai, menitSelesai);

    cout <<RESET<< endl;

    cout << "List ruangan tersedia dari " << formatHourMinute(mulai) << " sampai " << formatHourMinute(selesai) << ",\npada tanggal " << formatDate(mulai) << ":" <<RESET<< endl;
    cout << "---" <<RESET<< endl;
    bool found = false;
    for (const auto& pair : daftarRuangan) {
        if (pair.second.cekKetersediaan(mulai, selesai)) {
            cout << "ID: " << pair.second.getId() << ", Nama: " << pair.second.getNamaRuangan() <<RESET<< endl;
            found = true;
        }
    }
    if (!found) {
        cout << "Tidak ada." <<RESET<< endl;
    }
    cout << "---" <<RESET<< endl;
}

void Ruangan::ubahRuangan(string idRuanganLama, string idRuanganBaru, string idJadwal) {
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
    
    // Tambah jadwal di ruangan baru
    ruanganBaru.tambahJadwal(dataJadwal);
    // Hapus jadwal di ruangan lama
    ruanganLama.hapusJadwalById(idJadwal);

    cout << CYAN << "\nJadwal telah dipindah dari " << ruanganLama.getNamaRuangan() << " ke " << ruanganBaru.getNamaRuangan() << RESET << endl;
}

void ubahJadwal(unordered_map<string, Ruangan>& daftarRuangan) {
    int c;
    string targetIdJadwal, targetIdRuangan, namaKegiatan;
    Ruangan dummy;
    time_t mulai;
    time_t selesai;
    cout << '\n' << GREEN << "UPDATE JADWAL" << RESET<< endl;   
    printAllRuangan(); cout << endl;
    targetIdRuangan = searchJadwalRuangan(daftarRuangan);
    
    if(targetIdRuangan == " ") {
        cout << "Maaf, ruangan tidak ditemukan." << endl;
        return;
    }

    unordered_map<string, Jadwal> dataJadwal = daftarRuangan[targetIdRuangan].getJadwal();
    if(dataJadwal.empty()) {
        return;
    }

    cout << '\n' << ">> Pilih id jadwal yang ingin diubah: ";
    cin >> targetIdJadwal;
    
    // cout << GREEN << 
    
    cout << GREEN <<"\nPilih opsi berikut:" << RESET<< endl;
    cout << GREEN <<"1. Ubah waktu" << RESET << endl;
    cout << GREEN <<"2. Ubah nama kegiatan" << RESET << endl;
    cout << GREEN <<"3. Ubah ruangan" << RESET << endl;
    cout << RED <<"0. Batal" << RESET << endl;
    cout << ">> Pilih menu(0-3): ";
    cin >> c; cout << endl;
    
    if(c == 0) {
        return;
    }
    auto it = daftarRuangan.find(targetIdRuangan);
    switch(c) {
        case 1: {
            int thn, bln, hari, jamMulai, menitMulai, jamSelesai, menitSelesai;
            cout << ">> Tanggal (YYYY MM DD)\t: ";
            
            cin >> thn >> bln >> hari;
            cout << ">> Waktu mulai (HH MM)\t: ";
            cin >> jamMulai >> menitMulai;
            time_t mulai = makeTime(thn, bln, hari, jamMulai, menitMulai);
            cout << left << setw(26)<< ">> Waktu selesai (HH MM): ";
            cin >> jamSelesai >> menitSelesai;
            time_t selesai = makeTime(thn, bln, hari, jamSelesai, menitSelesai);
            cout <<RESET<< endl;

            it->second.ubahWaktu(targetIdJadwal, mulai, selesai);
        }   
            break;
        case 2: {
            string namaKegiatan;
            cout << ">> Masukkan nama kegiatan: " << RESET;
            getline(cin >> ws, namaKegiatan);

            it->second.ubahNamaKegiatan(targetIdJadwal, namaKegiatan);
            break;
        }
        case 3: {
            string idRuanganBaru;
            cout << ">> Masukkan ID ruangan baru: " << RESET;
            cin >> idRuanganBaru;
            it->second.ubahRuangan(targetIdRuangan, idRuanganBaru, targetIdJadwal);
            break;
        }
        default:
            cout << "Pilihan tidak valid. Silakan coba lagi." << RESET << endl;
    }
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
            cout << CYAN <<  "Jadwal berhasil ditambahkan." <<RESET<< endl;
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
        cout << "\n" << GREEN << "Menu:" << RESET <<RESET<< endl;
        cout << GREEN << "1. Lihat List Ruangan" << RESET  <<RESET<< endl;
        cout << GREEN << "2. Search Jadwal dari Ruangan" << RESET  <<RESET<< endl;
        cout << GREEN << "3. Search Jadwal dari Waktu" << RESET <<RESET<< endl;
        cout << GREEN << "4. Update Jadwal" << RESET <<RESET<< endl;
        cout << RED << "0. Selesai" << RESET <<RESET<< endl;

        int choice;
        cout << GREEN << "Pilih menu (0-4): " << RESET;
        if (!(cin >> choice)) {
            cout <<RESET<< endl;
            cout << RED << "ERR: Input harus angka." << RESET <<RESET<< endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 0) {
            break;
        }
        cout <<RESET<< endl;

        switch(choice) {
            case 1: {
                printAllRuangan();
                break;
            }
            case 2: {
                searchJadwalRuangan(daftarRuangan);
                break;
            }
            case 3: {
                searchRuanganTersedia(daftarRuangan);
                break;
            }
            case 4: {
                ubahJadwal(daftarRuangan);
                break;
            }
            case 0: {
                cout << "\nByeeee!" << endl;
                return 0;
            }
            default:
                cout << "Pilihan tidak valid. Silakan coba lagi." <<RESET<< endl;
                break;
        }
    }

    return 0;
}