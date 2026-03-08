# Sistem Manajemen Jadwal & Konflik Ruang

## Deskripsi Proyek

Proyek ini mengimplementasikan sistem manajemen jadwal penggunaan ruang (seperti ruang rapat, ruang kelas, atau coworking space). Sistem ini bertujuan untuk membantu organisasi mengelola penggunaan ruang secara efisien serta mendeteksi konflik jadwal secara otomatis.

Pada implementasi ini, data jadwal disimpan dalam **struktur data `map`** untuk memastikan operasi pencarian, update, dan penghapusan dapat dilakukan secara cepat dengan kompleksitas waktu **O(log n)**, sekaligus menjamin urutan data yang konsisten saat disimpan ke file JSON.

Untuk penyimpanan permanen (persistent storage), sistem menggunakan **file JSON (`dummy.json`)**. Saat program dijalankan, data dari JSON akan dimuat ke dalam memori, kemudian semua operasi sistem dilakukan pada struktur data di memori. Ketika program selesai, data terbaru akan disimpan kembali ke file JSON.

---

# Arsitektur Sistem

Sistem menggunakan pendekatan **runtime in-memory database + persistent JSON storage**.

Alur data sistem:

```
dummy.json (persistent storage)
        ↓
readJson()
        ↓
map<string, Ruangan>   ← runtime database
        ↓
Operasi sistem (insert, update, delete, search)
        ↓
writeJson()
        ↓
dummy.json diperbarui
```

Keuntungan pendekatan ini:

- Operasi data cepat selama runtime
- Urutan data konsisten di JSON karena `map` auto-sorted by key
- Penyimpanan data tetap tersedia setelah program ditutup
- Struktur kode modular dan mudah diperluas

---

# Struktur File

```
proyek/
├── main.cpp               — entry point, program loop
├── globals.h              — runtime storage & ID generation
├── utils.h                — helper functions (time formatting, input)
├── models/
│   ├── Jadwal.h           — class Jadwal
│   └── Ruangan.h          — class Ruangan
├── storage/
│   └── Storage.h          — readJson, writeJson, tambahJadwalBaru, ubahRuangan
├── ui/
│   └── Menu.h             — semua fungsi display & pagination
├── actions/
│   └── Actions.h          — semua operasi CRUD
└── dummy.json             — persistent storage
```

---

# Struktur Data

## 1. Ruangan

Setiap ruangan memiliki kumpulan jadwal yang disimpan menggunakan `map`.

```
Ruangan
│
├── idRuangan
├── namaRuangan
└── daftarJadwal (map<string, Jadwal>)
```

`map` digunakan agar jadwal tersimpan terurut by ID dan pencarian tetap efisien O(log n).

---

## 2. Jadwal

Setiap jadwal menyimpan informasi kegiatan yang menggunakan suatu ruangan.

```
Jadwal
│
├── idJadwal
├── namaKegiatan
├── waktuMulai (Unix timestamp)
└── waktuSelesai (Unix timestamp)
```

---

# Penyimpanan Data (JSON)

Data disimpan dalam file `dummy.json` dengan struktur berikut:

```json
{
  "ruangan": [
    {
      "id": "R001",
      "namaRuangan": "Ruang A",
      "jadwal": [
        {
          "id": "J000001",
          "namaKegiatan": "Meeting Tim",
          "mulai": 1771129800,
          "selesai": 1771133400
        }
      ]
    }
  ]
}
```

### Keterangan Field

| Field | Deskripsi |
|---|---|
| `id` | ID unik ruang |
| `namaRuangan` | Nama ruang |
| `jadwal` | Daftar jadwal pada ruang tersebut |
| `namaKegiatan` | Nama kegiatan |
| `mulai` | Waktu mulai (Unix timestamp) |
| `selesai` | Waktu selesai (Unix timestamp) |

Karena `map` auto-sorted, urutan ruangan dan jadwal di JSON selalu konsisten — git diff tetap bersih meskipun program dijalankan berulang kali tanpa perubahan data.

---

# Mekanisme Load & Save JSON

## Load Data (`readJson`)

1. Membaca file `dummy.json`
2. Parsing JSON menggunakan library **nlohmann/json**
3. Mengonversi data JSON menjadi objek `Ruangan` dan `Jadwal`
4. Menyimpannya ke dalam `map<string, Ruangan>`
5. Menentukan `globalJadwalCounter` berdasarkan ID jadwal tertinggi

## Save Data (`writeJson`)

1. Mengambil seluruh data dari `map` (sudah terurut)
2. Mengonversinya kembali menjadi format JSON
3. Menulis ulang file menggunakan `ios::trunc`

---

# Deteksi Konflik Jadwal

Konflik jadwal dideteksi menggunakan pengecekan interval waktu sederhana.

Dua jadwal dianggap konflik jika interval waktunya **tumpang tindih**.

```cpp
if (!(selesaiBaru <= mulaiLama || mulaiBaru >= selesaiLama))
    // konflik
```

Kompleksitas deteksi konflik: **O(n)** dengan `n` = jumlah jadwal pada ruangan tersebut.

---

# Fitur Sistem

## 1. Browsing List Ruangan
Menampilkan semua ruangan dengan pagination responsif. Jumlah kolom menyesuaikan lebar terminal. Navigasi menggunakan arrow key atau N/P/Q.

## 2. Tambah Jadwal
Menambahkan jadwal baru ke ruangan tertentu dengan pengecekan konflik waktu otomatis. Input tanggal mendukung shortcut `T` untuk hari ini.

## 3. Search Jadwal dari Ruangan
Menampilkan semua jadwal pada ruangan tertentu dengan pagination. Jadwal ditampilkan dari terbaru ke terlama. Jadwal yang sudah lewat ditampilkan abu-abu.

## 4. Search Jadwal berdasarkan Waktu
Mencari semua jadwal yang aktif (overlap) dalam rentang waktu tertentu lintas semua ruangan. Hasil ditampilkan dengan pagination.

## 5. Search Ruangan Tersedia
Mencari semua ruangan yang tidak memiliki konflik pada rentang waktu tertentu. Hasil ditampilkan dengan pagination responsif.

## 6. Update Jadwal
Memperbarui jadwal yang sudah ada, meliputi:
- Waktu mulai & selesai
- Nama kegiatan
- Perpindahan ruangan

## 7. Hapus Jadwal
Menghapus jadwal berdasarkan ID.

---

# Kompleksitas Operasi

| Operasi | Kompleksitas |
|---|---|
| Insert Jadwal | O(log n) |
| Delete Jadwal | O(log n) |
| Search Jadwal by ID | O(log n) |
| Deteksi Konflik | O(n) |
| Load JSON | O(N) |
| Save JSON | O(N) |

`n` = jumlah jadwal per ruangan, `N` = total jadwal dalam sistem.

---

# Sistem Pagination & Navigasi

Karena data ruangan dan jadwal bisa sangat banyak (ratusan hingga ribuan), sistem menggunakan pagination responsif pada semua fitur yang menampilkan list data.

## Cara Kerja

Jumlah item per halaman dihitung otomatis berdasarkan ukuran terminal yang dideteksi saat runtime menggunakan `sys/ioctl`:

```cpp
auto [termCols, termRows] = getTerminalSize();
int PAGE_SIZE = (termRows - 5) * cols;
```

Jumlah kolom juga menyesuaikan lebar terminal:

| Lebar Terminal | Kolom |
|---|---|
| < 60 karakter | 1 kolom |
| 60–119 karakter | 2 kolom |
| ≥ 120 karakter | 3 kolom |

## Navigasi

Semua halaman pagination menggunakan raw mode (`termios`) agar input langsung terbaca tanpa perlu menekan Enter:

| Tombol | Aksi |
|---|---|
| `→` / `↓` / `N` | Halaman berikutnya |
| `←` / `↑` / `P` | Halaman sebelumnya |
| `Q` | Keluar dari browser |

## Fungsi Pagination

| Fungsi | Digunakan untuk |
|---|---|
| `browseRuangan()` | Browsing semua ruangan (fitur 1) |
| `browseJadwal()` | Jadwal dalam satu ruangan (fitur 3) |
| `browseHasilSearch()` | Hasil search jadwal lintas ruangan (fitur 4) |
| `browseHasilRuangan()` | Hasil search ruangan tersedia (fitur 5) |

---


Folder `bin` telah ditambahkan ke `.gitignore`
```bash
g++ -I. -std=c++17 main.cpp -o ./bin/main && ./bin/main
```

Pastikan folder `nlohmann/` berisi `json.hpp` ada di direktori yang sama dengan `main.cpp`.

---

# Teknologi yang Digunakan

- **C++17**
- **STL** — `map`, `vector`, `tuple`, dll
- **nlohmann/json** — parsing JSON (https://github.com/nlohmann/json/)
- **Unix timestamp** — penyimpanan waktu
- **termios** — raw mode untuk arrow key navigation
- **sys/ioctl** — deteksi ukuran terminal untuk responsive layout

---

# Pengembangan Selanjutnya

Untuk tahap akhir proyek, sistem akan diperluas dengan:

- Implementasi **minimal dua struktur data berbeda** dan perbandingan performa
- Grafik perbandingan waktu eksekusi & penggunaan memori
- Analisis dampak pertumbuhan data terhadap performa
- Rekomendasi struktur data terbaik untuk sistem manajemen jadwal
