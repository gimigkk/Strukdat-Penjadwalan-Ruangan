# Sistem Manajemen Jadwal & Konflik Ruang

## Deskripsi Proyek

Proyek ini mengimplementasikan sistem manajemen jadwal penggunaan ruang (seperti ruang rapat, ruang kelas, atau coworking space). Sistem ini bertujuan untuk membantu organisasi mengelola penggunaan ruang secara efisien serta mendeteksi konflik jadwal secara otomatis.

Pada implementasi ini, data jadwal disimpan dalam **struktur data hash map (`unordered_map`)** untuk memastikan operasi pencarian, update, dan penghapusan dapat dilakukan secara cepat dengan kompleksitas waktu rata-rata **O(1)**.

Untuk penyimpanan permanen (persistent storage), sistem menggunakan **file JSON (`data.json`)**. Saat program dijalankan, data dari JSON akan dimuat ke dalam memori, kemudian semua operasi sistem dilakukan pada struktur data di memori. Ketika program selesai, data terbaru akan disimpan kembali ke file JSON.

---

# Arsitektur Sistem

Sistem menggunakan pendekatan **runtime in-memory database + persistent JSON storage**.

Alur data sistem:

```
data.json (persistent storage)
        ↓
readJson()
        ↓
unordered_map<string, Ruangan>   ← runtime database
        ↓
Operasi sistem (insert, update, delete, search)
        ↓
writeJson()
        ↓
data.json diperbarui
```

Keuntungan pendekatan ini:

- Operasi data sangat cepat selama runtime
- Penyimpanan data tetap tersedia setelah program ditutup
- Struktur kode lebih modular dan mudah diperluas

---

# Struktur Data

## 1. Ruangan

Setiap ruangan memiliki kumpulan jadwal yang disimpan menggunakan hash map.

```
Ruangan
│
├── idRuangan
├── namaRuangan
└── daftarJadwal (unordered_map<string, Jadwal>)
```

Hash map digunakan agar pencarian jadwal berdasarkan ID dapat dilakukan dengan cepat.

---

## 2. Jadwal

Setiap jadwal menyimpan informasi kegiatan yang menggunakan suatu ruangan.

```
Jadwal
│
├── idJadwal
├── namaKegiatan
├── waktuMulai
└── waktuSelesai
```

---

# Penyimpanan Data (JSON)

Data disimpan dalam file `data.json` dengan struktur berikut:

```json
{
  "ruangan": [
    {
      "id": "R001",
      "namaRuangan": "Ruang A",
      "jadwal": [
        {
          "id": "J001",
          "namaKegiatan": "Meeting Tim",
          "mulai": 1771129800,
          "selesai": 1771133400
        }
      ]
    },
    {
      "id": "R002",
      "namaRuangan": "Ruang B",
      "jadwal": []
    }
  ]
}
```

### Keterangan Field

| Field | Deskripsi |
|------|------|
| `id` | ID unik ruang |
| `namaRuangan` | Nama ruang |
| `jadwal` | Daftar jadwal pada ruang tersebut |
| `namaKegiatan` | Nama kegiatan |
| `mulai` | Waktu mulai (Unix timestamp) |
| `selesai` | Waktu selesai (Unix timestamp) |

---

# Mekanisme Load & Save JSON

## Load Data

Saat program dimulai, sistem memuat data dari JSON menggunakan fungsi:

```
readJson()
```

Langkah-langkah:

1. Membaca file `data.json`
2. Parsing JSON menggunakan library **nlohmann/json**
3. Mengonversi data JSON menjadi objek `Ruangan` dan `Jadwal`
4. Menyimpannya ke dalam `unordered_map`

---

## Save Data

Saat program selesai, data terbaru disimpan kembali menggunakan fungsi:

```
writeJson()
```

Langkah-langkah:

1. Mengambil seluruh data dari `unordered_map`
2. Mengonversinya kembali menjadi format JSON
3. Menulis ulang file `data.json`

File ditulis menggunakan mode:

```
ios::trunc
```

yang memastikan file lama dihapus sebelum data baru ditulis.

---

# Deteksi Konflik Jadwal

Konflik jadwal dideteksi menggunakan pengecekan interval waktu sederhana.

Dua jadwal dianggap konflik jika interval waktunya **tumpang tindih**.

Logika yang digunakan:

```cpp
if (!(selesaiBaru <= mulaiLama || mulaiBaru >= selesaiLama))
    konflik
```

Jika kondisi tersebut terpenuhi, maka jadwal baru tidak dapat ditambahkan.

Kompleksitas waktu deteksi konflik:

```
O(n)
```

dengan `n` adalah jumlah jadwal pada ruangan tersebut.

---

# Fitur Sistem

Sistem menyediakan fitur berikut:

## 1. Insert Jadwal
Menambahkan jadwal baru ke ruangan tertentu dengan pengecekan konflik waktu.

## 2. Search Jadwal

Pencarian jadwal dapat dilakukan berdasarkan:

- ID ruangan
- rentang waktu tertentu

## 3. Update Jadwal

Data jadwal dapat diperbarui, termasuk:

- waktu mulai
- waktu selesai
- nama kegiatan
- perpindahan ruangan

## 4. Delete Jadwal

Jadwal dapat dihapus menggunakan ID jadwal.

## 5. Deteksi Konflik

Sistem secara otomatis menolak jadwal baru yang memiliki konflik waktu dengan jadwal yang sudah ada pada ruangan yang sama.

## 6. Auto-Rename ID Jadwal

Jika ID jadwal yang dimasukkan sudah ada, sistem akan otomatis mengganti ID menjadi format berikut:

```
J001 → J001_2 → J001_3 → ...
```

Hal ini memastikan setiap jadwal memiliki ID unik tanpa mengganggu pengguna.

---

# Kompleksitas Operasi

| Operasi | Kompleksitas |
|------|------|
| Insert Jadwal | O(1) rata-rata |
| Delete Jadwal | O(1) rata-rata |
| Search Jadwal | O(1) rata-rata |
| Deteksi Konflik | O(n) |
| Load JSON | O(N) |
| Save JSON | O(N) |

`N` adalah jumlah total jadwal dalam sistem.

---

# Teknologi yang Digunakan

- **C++**
- **STL (Standard Template Library)**
- **unordered_map**
- **nlohmann/json** untuk parsing JSON (https://github.com/nlohmann/json/)
- **Unix timestamp** untuk penyimpanan waktu

---

# Dataset Awal

Dataset awal menggunakan data simulasi yang disimpan pada file:

```
data.json
```

Dataset ini dapat diperluas untuk pengujian performa dengan jumlah jadwal yang lebih besar.

---

# Pengembangan Selanjutnya

Untuk tahap akhir proyek, sistem akan diperluas dengan:

- Implementasi **minimal dua struktur data berbeda**
- Perbandingan performa antara struktur data
- Visualisasi grafik waktu eksekusi
- Analisis penggunaan memori
- Rekomendasi struktur data terbaik untuk sistem manajemen jadwal
