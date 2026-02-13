# Topik 5: Sistem Manajemen Jadwal & Konflik Ruang

**Konteks industri:** Perkantoran, kampus, coworking space

## Deskripsi

Kasus ini menggambarkan sistem pengelolaan jadwal penggunaan ruang (ruang rapat, ruang kelas, atau ruang kerja bersama) pada organisasi dengan banyak pengguna. Pada awalnya, pengaturan jadwal dilakukan secara manual atau menggunakan tabel sederhana sehingga masih dapat dikendalikan. Namun, seiring meningkatnya jumlah pengguna, variasi kegiatan, dan keterbatasan jumlah ruang, konflik jadwal sering terjadi dan sulit dideteksi secara cepat.

Sistem membutuhkan pendekatan struktur data yang efisien agar pencatatan jadwal, pencarian ketersediaan ruang, dan pendeteksian konflik dapat dilakukan secara cepat meskipun data jadwal bertambah. Pada topik ini, sistem tidak diwajibkan melakukan optimasi penjadwalan otomatis atau algoritma kompleks. Fokus utama adalah pada pengelolaan data jadwal dan deteksi konflik sederhana menggunakan struktur data yang tepat.

## Permasalahan di Lapangan

- Konflik penggunaan ruang sulit dideteksi ketika jumlah jadwal meningkat
- Pencarian jadwal ruang tertentu menjadi lambat jika data disimpan secara sederhana
- Update dan pembatalan jadwal membutuhkan proses pencarian yang efisien
- Sistem tidak mampu menangani pertumbuhan data jadwal secara stabil

## Tujuan Proyek

Membangun sistem manajemen jadwal dan deteksi konflik ruang yang efisien, serta membandingkan performa struktur data yang digunakan dalam pengelolaan data jadwal.

## Domain Data (Minimal)

- ID ruang
- Nama ruang
- ID jadwal
- Tanggal
- Waktu mulai
- Waktu selesai
- Nama kegiatan
- Status jadwal

## Aktivitas Sistem (Wajib)

- Insert data jadwal
- Search jadwal berdasarkan ruang atau waktu
- Update data jadwal
- Delete jadwal
- Deteksi konflik jadwal sederhana (waktu tumpang tindih pada ruang yang sama)

**Catatan:** Deteksi konflik cukup menggunakan pengecekan interval waktu, tanpa optimasi penjadwalan.

## Fokus Analisis

Analisis perbandingan struktur data terhadap:

- Kecepatan pencarian jadwal
- Efisiensi deteksi konflik jadwal
- Kemudahan insert, update, dan delete
- Dampak pertumbuhan jumlah jadwal terhadap performa sistem

## Target Deliverables

### Minggu ke-7 (Progress)

- Spesifikasi sistem & skenario penggunaan ruang
- Implementasi minimal 1 struktur data
- Dataset awal jadwal (dummy/simulasi)
- Pengujian awal insert dan pencarian jadwal

### Minggu ke-14 (Final)

- Implementasi lengkap minimal 2 struktur data
- Grafik perbandingan waktu eksekusi & penggunaan memori
- Demo aplikasi manajemen jadwal & konflik ruang
- Laporan analisis performa & rekomendasi struktur data

## Catatan Penting

Mahasiswa tidak diwajibkan mengimplementasikan algoritma penjadwalan atau optimasi konflik tingkat lanjut. Fokus penilaian adalah pada penggunaan struktur data dan analisis performanya dalam sistem manajemen jadwal.
