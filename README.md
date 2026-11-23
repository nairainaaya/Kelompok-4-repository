# Kelompok-4-repository
Tugas UAS

ANGGOTA KELOMPOK 4:

Nama Lengkap: Naira Rifa Inaaya
NPM         : 260810701100001

Nama Lengkap: Mikyal Balqis
NPM         : 260810701100017

Nama Lengkap: T. Rama Arif Saevic
NPM         : 260810701100029

Nama Lengkap: Ferrsen Austin Lowich
NPM         : 260810701100048

Aplikasi ini adalah program konsol berbasis menu untuk mengelola perpustakaan, ditulis dalam bahasa C dengan pola "muat ke memori, olah, lalu simpan ke file". Saat program dijalankan, main.c memuat seluruh data dari file teks ke array global di memori, yaitu katalog buku, daftar user, dan riwayat peminjaman. Setelah itu pengguna diminta login. Setiap perubahan data langsung disimpan kembali ke file .txt, sehingga file-file tersebut berfungsi sebagai database sederhana. Setelah logout, pengguna bisa login dengan akun lain tanpa menjalankan ulang program, dan saat program berakhir data disimpan sekali lagi serta memori dibebaskan.

Struktur File:
main.c		Titik masuk, alur login, dan menu Admin/User
auth.c/.h	Login: baca username dan password, cocokkan dengan data user, kembalikan role
data.c/.h	Lapisan data: struct Buku, User, Peminjaman, array global, baca/tulis file, alokasi memori dinamis (realloc)
buku.c/.h	CRUD buku, pencarian (ID dan judul), tampilan tabel berhalaman
sorting.c/.h	Bubble, insertion, dan selection sort dengan fungsi pembanding; mengurutkan salinan data, bukan data asli
transaksi.c/.h	Pinjam dan kembalikan buku, pinjaman saya, laporan transaksi admin
input.c/.h	Pembaca input aman: baca_int, baca_string, konfirmasi

File Data
Books.txt	spasi, dengan tanda petik untuk judul dan penulis	Sumber awal, hanya dibaca
katalog.txt	ocaid;title;author;publisher;year;page_count;stok	Dibuat otomatis dari Books.txt saat pertama jalan; stok awal 3
user.txt	username,password,role					Akun login
peminjaman.txt	id;username;ocaid;tanggal;status			Dibuat otomatis; status DIPINJAM atau KEMBALI

## Panduan Kompilasi dan Menjalankan Program


### Prasyarat
- Compiler **GCC** (di Windows bisa lewat MinGW)
- **Make** (di Windows dengan MinGW perintahnya `mingw32-make`)

### Persiapan
Pastikan file berikut berada di **folder yang sama** dengan file `.c` dan `.h`:
- `Books.txt` (dataset buku)
- `user.txt` (data akun login)

File `katalog.txt` dan `peminjaman.txt` akan dibuat otomatis oleh program saat pertama kali dijalankan.


### Kompilasi

**Dengan Makefile (disarankan):**
# Linux / macOS
make
# Windows (MinGW)
mingw32-make

Hasilnya berupa program `perpustakaan` (di Windows: `perpustakaan.exe`).

**Tanpa Makefile (manual):**
gcc -Wall -Wextra -o perpustakaan main.c auth.c buku.c data.c input.c sorting.c transaksi.c


### Menjalankan Program
# Linux / macOS
./perpustakaan
# Windows (PowerShell)
./perpustakaan.exe

Jalankan dari folder yang berisi `Books.txt` dan `user.txt`, karena kedua file dibaca lewat path relatif.


### Akun untuk Login

Daftar akun (username, password, dan role) tersimpan di file `user.txt` dengan format `username,password,role`. Gunakan salah satu akun di file tersebut untuk login. Akun dengan role `admin` masuk ke menu Admin, sedangkan role `user` masuk ke menu User.


### Membersihkan Hasil Kompilasi
make clean            # Linux / macOS
mingw32-make clean    # Windows (MinGW)

Perintah ini menghapus file `.o`, `.d`, dan program hasil kompilasi.


