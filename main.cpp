// note untuk struktur datanya:
// Ruangan punya daftar jadwal
// Jadwal punya waktu mulai, waktu selesai, nama kegiatan, id
// Ruangan di simpen pake hashmap biar gampang aksesnya O(1)    <- tapi gw ganti jadi map wkwkkwkw -gilang
// Key di hashmap itu id ruangan.

#include "storage/Storage.h"
#include "ui/Menu.h"
#include "actions/Actions.h"
#include "latency.h"

int main (){
    // Akses json
    string file = "dummy1.json";

    readJson(file);
    flushLatencyPrints(); // cetak Latency Report bulk-insert saat startup

    /// Program Loop
    while (true) {
        cout << "\n" << GREEN << "Menu:" << RESET <<RESET<< endl;
        cout << GREEN << "1. Browsing List Ruangan" << RESET  <<RESET<< endl;
        cout << GREEN << "2. Tambah Jadwal" << RESET  <<RESET<< endl;
        cout << GREEN << "3. Search Jadwal dari Ruangan" << RESET  <<RESET<< endl;
        cout << GREEN << "4. Search Jadwal berdasarkan Waktu" << RESET  <<RESET<< endl;
        cout << GREEN << "5. Search Ruangan Tersedia pada Waktu Tertentu" << RESET <<RESET<< endl;
        cout << GREEN << "6. Update Jadwal" << RESET <<RESET<< endl;
        cout << GREEN << "7. Hapus Jadwal" << RESET <<RESET<< endl;
        cout << RED << "0. Selesai" << RESET <<RESET<< endl;

        int choice;

        cout << GREEN << "Pilih menu (0-7): " << RESET;
        if (!(cin >> choice)) {
            cout <<RESET<< endl;
            cout << RED << "ERR: Input harus angka." << RESET <<RESET<< endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if(choice == 0) {
            break;
        }

        cout <<RESET<< endl;

        switch(choice) {
            case 1: {
                browseRuangan();
                flushLatencyPrints(); // cetak Latency Report setelah user selesai browse
                break;
            }
            case 2: {
                tambahJadwal(daftarRuangan);
                break;
            }
            case 3: {
                searchJadwalRuangan(daftarRuangan);
                break;
            }
            case 4: {
                searchJadwalByTime(daftarRuangan);
                break;
            }
            case 5: {
                searchRuanganTersedia(daftarRuangan);
                break;
            }
            case 6: {
                ubahJadwal(daftarRuangan);
                break;
            }
            case 7: {
                hapusJadwal(daftarRuangan);
                break;
            }
            default:
                cout << "Pilihan tidak valid. Silakan coba lagi." <<RESET<< endl;
                break;
        }
    }
    
    cout << CYAN << "\n--- Byeee :D (writing to json) ---" << RESET << endl;
    writeJson(file);

    printLatencySummary();

    return 0;
}