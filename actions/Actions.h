#ifndef ACTIONS_H
#define ACTIONS_H

#include <bits/stdc++.h>
#include "../globals.h"
#include "../ui/Menu.h"

using namespace std;

void tambahJadwal(map<string, Ruangan>& daftarRuangan) {

    string idRuangan;
    int jamMulai, menitMulai;
    int jamSelesai, menitSelesai;
    string namaKegiatan;

    cout << GRAY << "(Tidak tahu ID? Gunakan menu 1 untuk browse ruangan)" << RESET << endl;
    cout << ">> Masukkan ID ruangan: ";
    cin >> idRuangan;
    transform(idRuangan.begin(), idRuangan.end(), idRuangan.begin(), ::toupper);

    auto it = daftarRuangan.find(idRuangan);
    if(it == daftarRuangan.end()) {
        cout << RED << "Ruangan tidak ditemukan." << RESET << endl;
        return;
    }
    
    it->second.printAllJadwal();
    cout << endl;
    auto [thn, bln, hari] = inputTanggal();

    it->second.printJadwalByDate(makeTime(thn, bln, hari, 0, 0)); cout << endl;

    cout << ">> Waktu mulai (HH MM): ";
    cin >> jamMulai >> menitMulai;

    cout << ">> Waktu selesai (HH MM): ";
    cin >> jamSelesai >> menitSelesai;

    cout << ">> Nama kegiatan: ";
    getline(cin >> ws, namaKegiatan);

    time_t mulai = makeTime(thn, bln, hari, jamMulai, menitMulai);
    time_t selesai = makeTime(thn, bln, hari, jamSelesai, menitSelesai);

    if(mulai >= selesai) {
        cout << RED << "Waktu tidak valid." << RESET << endl;
        return;
    }

    it->second.tambahJadwalBaru(mulai, selesai, namaKegiatan);
}


// Ganti tipe fungsi ke string
string searchJadwalRuangan(const map<string, Ruangan>& daftarRuangan) { 
    cout << GRAY << "(Tidak tahu ID? Gunakan menu 1 untuk browse ruangan)" << RESET << endl;
    string searchId;
    cout << ">> Masukkan ID Ruangan yang ingin dicari: ";
    cin >> searchId;
    transform(searchId.begin(), searchId.end(), searchId.begin(), ::toupper);
    cout <<RESET<< endl;

    auto it = daftarRuangan.find(searchId);
    if (it != daftarRuangan.end()) {
        browseJadwal(it->second);
        return searchId;
    } else {
        cout << "Ruangan dengan ID " << searchId << " tidak ditemukan." <<RESET<< endl;
        return "";
    }
}

void searchJadwalByTime(const map<string, Ruangan>& daftarRuangan) {
    int jamMulai, menitMulai, jamSelesai, menitSelesai;
    auto [thn, bln, hari] = inputTanggal();

    cout << ">> Waktu mulai (HH MM)\t: ";
    cin >> jamMulai >> menitMulai;
    time_t mulai = makeTime(thn, bln, hari, jamMulai, menitMulai);

    cout << ">> Waktu selesai (HH MM)\t: ";
    cin >> jamSelesai >> menitSelesai;
    time_t selesai = makeTime(thn, bln, hari, jamSelesai, menitSelesai);

    if (mulai >= selesai) {
        cout << RED << "Waktu tidak valid." << RESET << endl;
        return;
    }

    cout << RESET << endl;

    vector<pair<const Jadwal*, string>> results;
    for (const auto& pair : daftarRuangan) {
        const auto& jadwalMap = pair.second.getJadwal();
        for (const auto& jadwalPair : jadwalMap) {
            const Jadwal& jadwal = jadwalPair.second;
            // cek overlap: jadwal aktif kalau ga selesai sebelum mulai dan ga mulai setelah selesai
            if (!(jadwal.getSelesai() <= mulai || jadwal.getMulai() >= selesai)) {
                results.push_back({&jadwal, pair.second.getNamaRuangan()});
            }
        }
    }

    if (results.empty()) {
        cout << RED << "Tidak ada jadwal pada range waktu tersebut." << RESET << endl;
        return;
    }

    string title = "Jadwal aktif " + formatHourMinute(mulai) + " - " + formatHourMinute(selesai)
                 + " pada " + formatDate(mulai);
    browseHasilSearch(title, results);
}

// Cari ruangan yang tersedia pada waktu tertentu
void searchRuanganTersedia(const map<string, Ruangan>& daftarRuangan) {
    int jamMulai, menitMulai, jamSelesai, menitSelesai;
    auto [thn, bln, hari] = inputTanggal();

    cout << ">> Waktu mulai (HH MM)\t: ";
    cin >> jamMulai >> menitMulai;
    time_t mulai = makeTime(thn, bln, hari, jamMulai, menitMulai);

    cout << left << setw(26)<< ">> Waktu selesai (HH MM): ";
    cin >> jamSelesai >> menitSelesai;
    time_t selesai = makeTime(thn, bln, hari, jamSelesai, menitSelesai);

    if (mulai >= selesai) {
        cout << RED << "Waktu tidak valid. Jam selesai harus setelah jam mulai." << RESET << endl;
        return;
    }

    cout << RESET << endl;

    vector<pair<string, string>> results;
    for (const auto& pair : daftarRuangan) {
        if (pair.second.cekKetersediaan(mulai, selesai)) {
            results.push_back({pair.second.getId(), pair.second.getNamaRuangan()});
        }
    }

    if (results.empty()) {
        cout << RED << "Tidak ada ruangan tersedia." << RESET << endl;
        return;
    }

    string title = "Ruangan tersedia " + formatHourMinute(mulai) + " - " + formatHourMinute(selesai)
                 + " pada " + formatDate(mulai);
    browseHasilRuangan(title, results);
}

void ubahJadwal(map<string, Ruangan>& daftarRuangan) {
    int c;
    string targetIdJadwal, targetIdRuangan, namaKegiatan;

    cout << '\n' << GREEN << "UPDATE JADWAL" << RESET<< endl;   
    targetIdRuangan = searchJadwalRuangan(daftarRuangan);
    
    if(targetIdRuangan.empty()) {
        cout << "Maaf, ruangan tidak ditemukan." << endl;
        return;
    }

    const auto& dataJadwal = daftarRuangan[targetIdRuangan].getJadwal();

    if(dataJadwal.empty()) {
        return;
    }

    cout << '\n' << ">> Pilih id jadwal yang ingin diubah: ";
    cin >> targetIdJadwal;
    
    
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
            int jamMulai, menitMulai, jamSelesai, menitSelesai;
            auto [thn, bln, hari] = inputTanggal();
            cout << ">> Waktu mulai (HH MM)\t: ";
            cin >> jamMulai >> menitMulai;
            time_t mulai = makeTime(thn, bln, hari, jamMulai, menitMulai);
            cout << left << setw(26)<< ">> Waktu selesai (HH MM): ";
            cin >> jamSelesai >> menitSelesai;
            time_t selesai = makeTime(thn, bln, hari, jamSelesai, menitSelesai);
            cout <<RESET<< endl;

            if (mulai >= selesai) {
                cout << RED << "Waktu tidak valid. Jam selesai harus setelah jam mulai." << RESET << endl;
                break;
            }

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
            transform(idRuanganBaru.begin(), idRuanganBaru.end(), idRuanganBaru.begin(), ::toupper);
            it->second.ubahRuangan(targetIdRuangan, idRuanganBaru, targetIdJadwal);
            break;
        }
        default:
            cout << "Pilihan tidak valid. Silakan coba lagi." << RESET << endl;
    }
}

void hapusJadwal(map<string, Ruangan>& daftarRuangan) {
    string targetIdRuangan, targetIdJadwal;

    cout << '\n' << GREEN << "HAPUS JADWAL" << RESET<< endl;   
    targetIdRuangan = searchJadwalRuangan(daftarRuangan);
    
    if(targetIdRuangan.empty()) {
        cout << "Maaf, ruangan tidak ditemukan." << endl;
        return;
    }

    const auto& dataJadwal = daftarRuangan[targetIdRuangan].getJadwal();

    if(dataJadwal.empty()) {
        return;
    }

    cout << '\n' << ">> Pilih id jadwal yang ingin dihapus: ";
    cin >> targetIdJadwal;

    auto it = daftarRuangan.find(targetIdRuangan);
    if(it->second.hapusJadwalById(targetIdJadwal)) {
        cout << CYAN << "\nJadwal dengan ID " << targetIdJadwal << " di ruangan " << it->second.getNamaRuangan() << " berhasil dihapus." << RESET << endl;
    } 
    else {
        cout << RED << "\nID jadwal tidak ditemukan." << RESET << endl;
    }
}

#endif