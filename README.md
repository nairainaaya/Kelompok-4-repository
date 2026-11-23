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


