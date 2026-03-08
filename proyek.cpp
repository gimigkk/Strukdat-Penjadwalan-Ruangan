// note untuk struktur datanya:
// Ruangan punya daftar jadwal
// Jadwal punya waktu mulai, waktu selesai, nama kegiatan, id
// Ruangan di simpen pake hashmap biar gampang aksesnya O(1)
// Key di hashmap itu id ruangan.

#include <bits/stdc++.h>
#include "utils.h" 
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;


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

        void tambahJadwalBaru(time_t mulai, time_t selesai, const string& namaKegiatan);
        void ubahRuangan(const string& idRuanganLama, const string& idRuanganBaru, const string& idJadwal);        
        void loadJadwal(const Jadwal& j);
        void printJadwalByDate(time_t targetDate) const;

};


// runtime storage kita
// buat presistent storage kita simpen di file
// tapi pikirin lagi karena gw ga kepikiran.
unordered_map<string, Ruangan> daftarRuangan;

// Auto generate ID
int globalJadwalCounter = 1;

string generateJadwalId() {
    stringstream ss;
    ss << "J" << setw(6) << setfill('0') << globalJadwalCounter++;
    return ss.str();
}

int extractJadwalNumber(const string& id) {
    if (id.size() < 2 || id[0] != 'J') return 0;
    return stoi(id.substr(1));
}

bool isJadwalIdExistGlobal(const string& idJadwal) {
    for (const auto& pair : daftarRuangan) {
        const auto& jadwalMap = pair.second.getJadwal();
        if (jadwalMap.find(idJadwal) != jadwalMap.end()) {
            return true;
        }
    }
    return false;
}

string makeUniqueJadwalIdGlobal(const string& baseId) {
    if (!isJadwalIdExistGlobal(baseId)) return baseId;

    for (int i = 2; ; i++) {
        string candidate = baseId + "_" + to_string(i);
        if (!isJadwalIdExistGlobal(candidate)) {
            return candidate;
        }
    }
}

// Calvin:
// Persistent storagenya pakai data.json 
// Di sini, rencananya : load at start, save at exit
// Perubahan data disimpen di hash map dulu
// Nanti data.json nya tinggal di overwrite pake data terbaru dari hash map

void Ruangan::loadJadwal(const Jadwal& j) {
    daftarJadwal[j.getIdJadwal()] = j;
}
// Fungsi buat read JSON
void readJson(const string& file) {
    json data;
    ifstream File(file);

    if(!File.is_open()) {
        cout << RED << "File not found" << endl;
        return;
    } 
    // Parse 
    File >> data;
    File.close();

    // Bersihin runtime storage
    daftarRuangan.clear();

    int maxJadwalNumber = 0;

    // Masukkin ke map
    for(auto& ruangan : data["ruangan"]) {
        string id = ruangan["id"];
        string namaRuangan = ruangan["namaRuangan"];

        Ruangan dataRuangan(namaRuangan, id);

        for(auto& jadwal : ruangan["jadwal"]) {
            string id = jadwal["id"];
            string namaKegiatan = jadwal["namaKegiatan"];
            time_t mulai = jadwal["mulai"];
            time_t selesai = jadwal["selesai"];

            Jadwal dataJadwal(mulai, selesai, namaKegiatan, id);
            dataRuangan.loadJadwal(dataJadwal);

            maxJadwalNumber = max(maxJadwalNumber, extractJadwalNumber(id));
        }
        daftarRuangan.insert({dataRuangan.getId(), dataRuangan});
    }
    globalJadwalCounter = maxJadwalNumber + 1;
}

// Buat masukkin data ke json
void writeJson(const string& file) {
    json data;

    // Bikin array of ruangan
    data["ruangan"] = json::array();
    for(auto& r : daftarRuangan) {
        Ruangan& ruangan = r.second;
        json dr; // data ruangan

        dr["id"] = ruangan.getId();
        dr["namaRuangan"] = ruangan.getNamaRuangan();

        // bikin array of jadwal
        dr["jadwal"] = json::array();

        for(auto& j: ruangan.getJadwal()) {
            const Jadwal& jadwal = j.second;
            
            json dj; // data jadwal

            dj["id"] = jadwal.getIdJadwal();
            dj["namaKegiatan"] = jadwal.getNamaKegiatan();
            dj["mulai"] = jadwal.getMulai();
            dj["selesai"] = jadwal.getSelesai();

            dr["jadwal"].push_back(dj);
        }
        data["ruangan"].push_back(dr);

    }
    // Write dengan ditruncate dlu 
    ofstream File(file, ios::trunc); 
    File << data.dump(4);
}

// Print semua ruangan
void printAllRuangan() {
    cout << "Daftar Ruangan:" << endl;
    for (const auto& pair : daftarRuangan) {
        cout << "ID: " << pair.second.getId() << ", Nama: " << pair.second.getNamaRuangan() << endl;
    }
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

void Ruangan::tambahJadwalBaru(time_t mulai, time_t selesai, const string& namaKegiatan) {
    if (!cekKetersediaan(mulai, selesai)) {
        cout << RED << "Jadwal bentrok dengan jadwal lain." << RESET << endl;
        return;
    }

    string newId = generateJadwalId();
    Jadwal j(mulai, selesai, namaKegiatan, newId);
    setJadwal(j);

    cout << CYAN << "Jadwal berhasil ditambahkan dengan ID " << newId << RESET << endl;
}

void tambahJadwal(unordered_map<string, Ruangan>& daftarRuangan) {

    string idRuangan;
    int thn, bln, hari;
    int jamMulai, menitMulai;
    int jamSelesai, menitSelesai;
    string namaKegiatan;

    printAllRuangan(); cout << endl;
    cout << ">> Masukkan ID ruangan: ";
    cin >> idRuangan;

    auto it = daftarRuangan.find(idRuangan);
    if(it == daftarRuangan.end()) {
        cout << RED << "Ruangan tidak ditemukan." << RESET << endl;
        return;
    }
    
    it->second.printAllJadwal();
    cout << endl;
    cout << ">> Tanggal (YYYY MM DD): ";
    cin >> thn >> bln >> hari;

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
string searchJadwalRuangan(const unordered_map<string, Ruangan>& daftarRuangan) { 
    printAllRuangan(); cout << endl;
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
        return "";
    }
}

void searchJadwalByTime(const unordered_map<string, Ruangan>& daftarRuangan) {
    int thn, bln, hari, jamMulai, menitMulai;
    cout << ">> Tanggal (YYYY MM DD)\t: ";
    cin >> thn >> bln >> hari;

    cout << ">> Waktu mulai (HH MM)\t: ";
    cin >> jamMulai >> menitMulai;
    time_t mulai = makeTime(thn, bln, hari, jamMulai, menitMulai);

    cout <<RESET<< endl;

    cout << "Jadwal yang dimulai pada " << formatHourMinute(mulai) << ",\npada tanggal " << formatDate(mulai) << ":" <<RESET<< endl;
    cout << "---" <<RESET<< endl;
    bool found = false;
    for (const auto& pair : daftarRuangan) {
        const auto& jadwalMap = pair.second.getJadwal();
        for (const auto& jadwalPair : jadwalMap) {
            const Jadwal& jadwal = jadwalPair.second;
            if (jadwal.getMulai() == mulai) {
                cout << "ID Ruangan\t: " << pair.second.getId() << endl;
                cout << "Nama Ruangan\t: " << pair.second.getNamaRuangan() << endl;
                cout << "ID Jadwal\t: " << jadwal.getIdJadwal() << endl;
                cout << "Kegiatan\t: " << jadwal.getNamaKegiatan() << endl;
                cout << "Jam Mulai\t: " << formatTime(jadwal.getMulai()) << endl;
                cout << "Jam Selesai\t: " << formatTime(jadwal.getSelesai()) << endl;
                found = true;
            }
        }
    }
    if (!found) {
        cout << RED << "Tidak ada." <<RESET<< endl;
    }
    cout << "---" <<RESET<< endl;

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

    if (mulai >= selesai) {
        cout << RED << "Waktu tidak valid. Jam selesai harus setelah jam mulai." << RESET << endl;
        return;
    }   

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
        cout << RED << "Tidak ada." <<RESET<< endl;
    }
    cout << "---" <<RESET<< endl;
}

void Ruangan::ubahRuangan(const string& idRuanganLama, const string& idRuanganBaru, const string& idJadwal) {
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

    if (ruanganBaru.getJadwal().find(idJadwal) != ruanganBaru.getJadwal().end()) {
        cout << "ID jadwal sudah ada di ruangan baru. Proses pembaruan dibatalkan.\n";
        return;
    }

    if (!ruanganBaru.cekKetersediaan(dataJadwal.getMulai(), dataJadwal.getSelesai())) {
        cout << "Ruangan baru tidak tersedia.\n";
        return;
    }  

    // Tambah jadwal di ruangan baru
    ruanganBaru.setJadwal(dataJadwal);
    // Hapus jadwal di ruangan lama
    ruanganLama.hapusJadwalById(idJadwal);

    cout << CYAN << "\nJadwal telah dipindah dari " << ruanganLama.getNamaRuangan() << " ke " << ruanganBaru.getNamaRuangan() << RESET << endl;
}

void ubahJadwal(unordered_map<string, Ruangan>& daftarRuangan) {
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
            it->second.ubahRuangan(targetIdRuangan, idRuanganBaru, targetIdJadwal);
            break;
        }
        default:
            cout << "Pilihan tidak valid. Silakan coba lagi." << RESET << endl;
    }
}

void hapusJadwal(unordered_map<string, Ruangan>& daftarRuangan) {
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

int main (){
    // Akses json
    string file = "dummy.json";

    readJson(file);

    /// Program Loop
    while (true) {
        cout << "\n" << GREEN << "Menu:" << RESET <<RESET<< endl;
        cout << GREEN << "1. Lihat List Ruangan" << RESET  <<RESET<< endl;
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
                printAllRuangan();
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
    
    cout << "\nByeeee!" << endl;
    writeJson(file);

    return 0;
}