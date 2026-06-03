
#include "storage/Storage2.h"
#include "ui/Menu2.h"
#include "actions/Actions2.h"
#include "latency.h"

int main (){
    // Akses json
    const char* dataFile = getenv("DATA_FILE");
    string file = (dataFile && dataFile[0] != '\0') ? dataFile : "dummy1.json";
    cout << GRAY << "Data file: " << file << RESET << "\n";

    recordMemoryUsage("vector::startup baseline");
    readJson(file);
    recordMemoryUsage("vector::after readJson");
    flushLatencyPrints(); // cetak Latency Report bulk-insert saat startup
    flushMemoryPrints();

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

        string memoryCheckpoint;
        switch(choice) {
            case 1: {
                browseRuangan();
                memoryCheckpoint = "vector::after browseRuangan";
                break;
            }
            case 2: {
                tambahJadwal(daftarRuangan);
                memoryCheckpoint = "vector::after tambahJadwal";
                break;
            }
            case 3: {
                searchJadwalRuangan(daftarRuangan);
                memoryCheckpoint = "vector::after searchJadwalRuangan";
                break;
            }
            case 4: {
                searchJadwalByTime(daftarRuangan);
                memoryCheckpoint = "vector::after searchJadwalByTime";
                break;
            }
            case 5: {
                searchRuanganTersedia(daftarRuangan);
                memoryCheckpoint = "vector::after searchRuanganTersedia";
                break;
            }
            case 6: {
                ubahJadwal(daftarRuangan);
                memoryCheckpoint = "vector::after ubahJadwal";
                break;
            }
            case 7: {
                hapusJadwal(daftarRuangan);
                memoryCheckpoint = "vector::after hapusJadwal";
                break;
            }
            default:
                cout << "Pilihan tidak valid. Silakan coba lagi." <<RESET<< endl;
                break;
        }

        if (!memoryCheckpoint.empty()) {
            recordMemoryUsage(memoryCheckpoint);
            flushLatencyPrints();
            flushMemoryPrints();
        }
    }
    
    cout << CYAN << "\n--- Byeee :D (writing to json) ---" << RESET << endl;
    writeJson(file);
    recordMemoryUsage("vector::after writeJson");
    flushLatencyPrints();
    flushMemoryPrints();

    printLatencySummary();
    printMemorySummary();

    return 0;
}
