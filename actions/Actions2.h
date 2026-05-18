#ifndef ACTIONS_H
#define ACTIONS_H

#include <bits/stdc++.h>
#include "../globals2.h"
#include "../ui/Menu2.h"
#include "../latency.h"

using namespace std;

void tambahJadwal(vector<Ruangan>& daftarRuangan) {

    string idRuangan;
    int jamMulai, menitMulai;
    int jamSelesai, menitSelesai;
    string namaKegiatan;

    cout << GRAY << "(Tidak tahu ID? Gunakan menu 1 untuk browse ruangan)" << RESET << endl;
    cout << ">> Masukkan ID ruangan: ";
    cin >> idRuangan;
    transform(idRuangan.begin(), idRuangan.end(), idRuangan.begin(), ::toupper);

    // Ukur waktu pencarian ruangan di vector
    ScopeTimer tFind("vector::find (tambahJadwal)");
    Ruangan* it = findRuangan(idRuangan);
    tFind.stop();

    if(it == nullptr) {
        cout << RED << "Ruangan tidak ditemukan." << RESET << endl;
        return;
    }
    
    cout << endl;
    auto [thn, bln, hari] = inputTanggal();

    // show jadwal ruangan pada tanggal itu biar user bisa liat ketersediaan
    browseJadwalByDate(*it, makeTime(thn, bln, hari, 0, 0)); 
    
    cout << GREEN << "Tambah Jadwal Baru" << RESET << endl;

    cout << ">> Waktu mulai (HH MM)\t: ";
    cin >> jamMulai >> menitMulai;

    cout << ">> Waktu selesai (HH MM): ";
    cin >> jamSelesai >> menitSelesai;

    cout << ">> Nama kegiatan\t: ";
    getline(cin >> ws, namaKegiatan);

    time_t mulai = makeTime(thn, bln, hari, jamMulai, menitMulai);
    time_t selesai = makeTime(thn, bln, hari, jamSelesai, menitSelesai);

    if(mulai >= selesai) {
        cout << RED << "Waktu tidak valid." << RESET << endl;
        return;
    }

    if(!it->cekKetersediaan(mulai, selesai)) {
        cout << RED << "Jadwal bentrok dengan jadwal lain di ruangan ini." << RESET << endl;
        return;
    }

    // insert di-timer di dalam tambahJadwalBaru (Storage.h)
    it->tambahJadwalBaru(mulai, selesai, namaKegiatan);
    flushLatencyPrints(); // cetak Latency Report: setelah operasi selesai
}


// Ganti tipe fungsi ke string
string searchJadwalRuangan(const vector<Ruangan>& daftarRuangan) { 
    cout << GRAY << "(Tidak tahu ID? Gunakan menu 1 untuk browse ruangan)" << RESET << endl;
    string searchId;
    cout << ">> Masukkan ID Ruangan yang ingin dicari: ";
    cin >> searchId;
    transform(searchId.begin(), searchId.end(), searchId.begin(), ::toupper);
    cout <<RESET<< endl;

    // Ukur waktu pencarian ruangan di vector
    ScopeTimer tFind("vector::find (searchJadwalRuangan)");
    const Ruangan* it = findRuanganConst(searchId);
    tFind.stop();

    if (it != nullptr) {
        browseJadwal(*it);
        flushLatencyPrints(); // cetak Latency Report: setelah user selesai browse
        return searchId;
    } else {
        cout << "Ruangan dengan ID " << searchId << " tidak ditemukan." <<RESET<< endl;
        flushLatencyPrints(); // cetak Latency Report: setelah operasi selesai
        return "";
    }
}

void searchJadwalByTime(const vector<Ruangan>& daftarRuangan) {
    int jamMulai, menitMulai, jamSelesai, menitSelesai;
    auto [thn, bln, hari] = inputTanggal();

    cout << ">> Waktu mulai (HH MM)\t: ";
    cin >> jamMulai >> menitMulai;
    time_t mulai = makeTime(thn, bln, hari, jamMulai, menitMulai);

    cout << ">> Waktu selesai (HH MM): ";
    cin >> jamSelesai >> menitSelesai;
    time_t selesai = makeTime(thn, bln, hari, jamSelesai, menitSelesai);

    if (mulai >= selesai) {
        cout << RED << "Waktu tidak valid." << RESET << endl;
        return;
    }

    cout << RESET << endl;

    // Ukur waktu traversal seluruh vector jadwal
    ScopeTimer t("vector::scan (searchJadwalByTime)");
    vector<pair<const Jadwal*, string>> results;
    for (const auto& ruangan : daftarRuangan) {
        for (const auto& jadwal : ruangan.getJadwal()) {
            // cek overlap: jadwal aktif kalau ga selesai sebelum mulai dan ga mulai setelah selesai
            if (!(jadwal.getSelesai() <= mulai || jadwal.getMulai() >= selesai)) {
                results.push_back({&jadwal, ruangan.getNamaRuangan()});
            }
        }
    }
    t.stop(); // stop sebelum browse UI memblokir input

    if (results.empty()) {
        cout << RED << "Tidak ada jadwal pada range waktu tersebut." << RESET << endl;
        flushLatencyPrints();
        return;
    }

    string title = "Jadwal aktif " + formatHourMinute(mulai) + " - " + formatHourMinute(selesai)
                 + " pada " + formatDate(mulai);
    browseHasilSearch(title, results);
    flushLatencyPrints(); // cetak Latency Report: setelah user selesai browse
}

// Cari ruangan yang tersedia pada waktu tertentu
void searchRuanganTersedia(const vector<Ruangan>& daftarRuangan) {
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

    // Ukur waktu traversal seluruh vector ruangan
    ScopeTimer t("vector::scan (searchRuanganTersedia)");
    vector<pair<string, string>> results;
    for (const auto& ruangan : daftarRuangan) {
        if (ruangan.cekKetersediaan(mulai, selesai)) {
            results.push_back({ruangan.getId(), ruangan.getNamaRuangan()});
        }
    }
    t.stop(); // stop sebelum browse UI memblokir input

    if (results.empty()) {
        cout << RED << "Tidak ada ruangan tersedia." << RESET << endl;
        flushLatencyPrints();
        return;
    }

    string title = "Ruangan tersedia " + formatHourMinute(mulai) + " - " + formatHourMinute(selesai)
                 + " pada " + formatDate(mulai);
    browseHasilRuangan(title, results);
    flushLatencyPrints(); // cetak Latency Report: setelah user selesai browse
}

void ubahJadwal(vector<Ruangan>& daftarRuangan) {
    int c;
    string targetIdJadwal, targetIdRuangan, namaKegiatan;

    cout << '\n' << GREEN << "UPDATE JADWAL" << RESET<< endl;   
    targetIdRuangan = searchJadwalRuangan(daftarRuangan);
    cout << '\n' << GREEN << "UPDATE JADWAL" << RESET<< endl;   
    
    if(targetIdRuangan.empty()) {
        cout << "Maaf, ruangan tidak ditemukan." << endl;
        return;
    }

    //vector
    Ruangan* it = findRuangan(targetIdRuangan);
    if (!it || it->getJadwal().empty()) {
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

            // Ukur waktu update field jadwal di vector
            ScopeTimer t("vector::update (ubahWaktu)");
            it->ubahWaktu(targetIdJadwal, mulai, selesai);
            t.stop();
            flushLatencyPrints(); // cetak Latency Report: setelah operasi selesai
        }   
            break;
        case 2: {
            string namaKegiatan;
            cout << ">> Masukkan nama kegiatan: " << RESET;
            getline(cin >> ws, namaKegiatan);

            // Ukur waktu update field jadwal di vector
            ScopeTimer t("vector::update (ubahNamaKegiatan)");
            it->ubahNamaKegiatan(targetIdJadwal, namaKegiatan);
            t.stop();
            flushLatencyPrints(); // cetak Latency Report: setelah operasi selesai
            break;
        }
        case 3: {
            browseRuangan();
            string idRuanganBaru;
            cout << ">> Masukkan ID ruangan baru: " << RESET;
            cin >> idRuanganBaru;
            transform(idRuanganBaru.begin(), idRuanganBaru.end(), idRuanganBaru.begin(), ::toupper);

            // insert+erase di-timer di dalam ubahRuangan (Storage.h)
            it->ubahRuangan(targetIdRuangan, idRuanganBaru, targetIdJadwal);
            flushLatencyPrints(); // cetak Latency Report: setelah operasi selesai
            break;
        }
        default:
            cout << "Pilihan tidak valid. Silakan coba lagi." << RESET << endl;
    }
}

void hapusJadwal(vector<Ruangan>& daftarRuangan) {
    string targetIdRuangan, targetIdJadwal;

    cout << '\n' << GREEN << "HAPUS JADWAL" << RESET<< endl;   
    targetIdRuangan = searchJadwalRuangan(daftarRuangan);
    
    if(targetIdRuangan.empty()) {
        cout << "Maaf, ruangan tidak ditemukan." << endl;
        return;
    }

    //vector
    Ruangan* it = findRuangan(targetIdRuangan);
    if (!it || it->getJadwal().empty()) {
        return;
    }

    cout << '\n' << ">> Pilih id jadwal yang ingin dihapus: ";
    cin >> targetIdJadwal;

    // Ukur waktu erase dari vector jadwal
    ScopeTimer t("vector::erase (hapusJadwal)");
    bool berhasil = it->hapusJadwalById(targetIdJadwal);
    t.stop();
 
    if(berhasil) {
        cout << CYAN << "\nJadwal dengan ID " << targetIdJadwal << " di ruangan " << it->getNamaRuangan() << " berhasil dihapus." << RESET << endl;
    }
    else {
        cout << RED << "\nID jadwal tidak ditemukan." << RESET << endl;
    }
    flushLatencyPrints(); // cetak Latency Report: setelah operasi selesai
}

#endif
