#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "data.h"

#define FILE_ASLI       "Books.txt"
#define FILE_KATALOG    "katalog.txt"
#define FILE_USER       "user.txt"
#define FILE_PEMINJAMAN "peminjaman.txt"

#define STOK_AWAL       3
#define PANJANG_BARIS   4096
#define KAPASITAS_AWAL  64

//variabel global
Buku *daftar_buku = NULL; //isi daftar_buku adalah kumpulan struct buku. jadi untuk array daftar_buku[i] adalah struct buku ke-i
int jumlah_buku = 0;
User *daftar_user = NULL;
int jumlah_user = 0;
Peminjaman *daftar_pinjam = NULL;
int jumlah_pinjam = 0;

//variabel lokal yang nilainya tetap setelah diubah
static int kap_buku = 0;
static int kap_user = 0;
static int kap_pinjam = 0;

//fungsi bantuan untuk menyalin string dengan aman, akan menutup string dengan \0 diakhir.
static void salin(char *tujuan, const char *sumber, size_t ukuran) {
    strncpy(tujuan, sumber, ukuran - 1);
    tujuan[ukuran - 1] = '\0';
}

//fungsi bantuan untuk membuang \n dan \r, strcspn digunakan untuk mencari posisi mereka
static void buang_newline(char *baris) {
    baris[strcspn(baris, "\r\n")] = '\0';
}

//fungsi bantuan untuk mengecek baris kosong dan melewatinya. 
//Program memecah tiap baris berdasarkan pemisah, lalu mengharapkan 
//jumlah kolom tertentu (3 untuk user, 6 untuk katalog). 
//Baris kosong tidak punya kolom sama sekali, jadi tanpa pengecekan ini 
//program bisa rusak
static int baris_kosong(const char *baris) {
    while (*baris) {
        if (!isspace((unsigned char)*baris)) {
            return 0;
        }
        baris++;
    }
    return 1;
}

//fungsi menulis ke file dengan pemisah tertentu
static void tulis_aman(FILE *f, const char *teks, char pemisah) {
    for (; *teks != '\0'; teks++) {
        fputc(*teks == pemisah ? ' ' : *teks, f);
    }
}

//memecah satu baris menjadi beberapa kolom dengan pemisah tertentu, maks batas kolom
static int pecah(char *baris, char pemisah, char *kolom[], int maks) {
    int n = 0;
    char *c;
    kolom[n++] = baris;
    for (c = baris; *c != '\0'; c++) {
        if (*c == pemisah) {
            if (n < maks) {
                *c = '\0';
                kolom[n] = c + 1;
            }
            n++;
        }
    }
    return n;
}


//mengambil 1 kata dari suatu baris di books.txt, mengecek jika tanda petik
static int ambil_token(char **p, char *hasil, int ukuran) {
    int i = 0;

    while (**p != '\0' && isspace((unsigned char)**p)) {
        (*p)++;
    }
    if (**p == '\0') {
        return 0;
    }

    if (**p == '"') {
        (*p)++;
        while (**p != '\0') {
            if (**p == '"') {
                if ((*p)[1] == '"') {
                    if (i < ukuran - 1) { hasil[i++] = '"'; }
                    (*p) += 2;
                    continue;
                }
                break;
            }
            if (i < ukuran - 1) { hasil[i++] = **p; }
            (*p)++;
        }
        if (**p == '"') { (*p)++; }
    } else {
        while (**p != '\0' && !isspace((unsigned char)**p)) {
            if (i < ukuran - 1) {
                hasil[i++] = **p;
            }
            (*p)++;
        }
    }
    hasil[i] = '\0';
    return 1; //return 1 menandakan 1 token berhasil diambil
}

//mengecek apakah string berupa bilangan bulat 
//(contohnya digunakan pada penulisan tahun "1980")
static int ke_int(const char *teks, int *hasil) {
    char *akhir;
    long nilai;

    if (teks[0] == '\0') {
        return 0;
    }
    nilai = strtol(teks, &akhir, 10);
    if (*akhir != '\0') {
        return 0;
    }
    *hasil = (int)nilai;
    return 1;
}

//fungsi pembersih adar tak terjadi memori leak 
//agar data baru tak menumpuk dengan data lama fungsi
static void kosongkan_buku(void) {
    free(daftar_buku);
    daftar_buku = NULL;
    jumlah_buku = 0;
    kap_buku = 0;
}

static void kosongkan_user(void) {
    free(daftar_user);
    daftar_user = NULL;
    jumlah_user = 0;
    kap_user = 0;
}

static void kosongkan_peminjaman(void) {
    free(daftar_pinjam);
    daftar_pinjam = NULL;
    jumlah_pinjam = 0;
    kap_pinjam = 0;
}

//menambah 1 buku ke array daftar_buku, dan memperbesar array jika penuh
int tambah_buku_ke_array(const Buku *b) {
    if (jumlah_buku >= kap_buku) { //cek jika sudah penuh 
        int kap_baru = (kap_buku == 0) ? KAPASITAS_AWAL : kap_buku * 2; //menambah kapasitas, jika masih kosong, = kapasitas awal
        Buku *tmp = realloc(daftar_buku, kap_baru * sizeof(Buku)); //minta memori yang ditampung di tmp dulu, kenapa:
        if (tmp == NULL) { //untuk mengantisipasi jika error begini.
            printf("Error: memori tidak cukup untuk data buku.\n");
            return 0;
        }
        daftar_buku = tmp; //jika berhasil, daftar_buku akan menunjuk ke alamat memori baru
        kap_buku = kap_baru; //kapasitas juga diperbarui
    }
    daftar_buku[jumlah_buku++] = *b; //salin isi buku (dengan pointer yang menunjuknya) ke posisi kosong berikutnya, jumlah buku ditambah 1
    return 1;
}

static int tambah_user_ke_array(const User *u) {
    if (jumlah_user >= kap_user) {
        int kap_baru = (kap_user == 0) ? KAPASITAS_AWAL : kap_user * 2;
        User *tmp = realloc(daftar_user, kap_baru * sizeof(User));
        if (tmp == NULL) {
            printf("Error: memori tidak cukup untuk data user.\n");
            return 0;
        }
        daftar_user = tmp;
        kap_user = kap_baru;
    }
    daftar_user[jumlah_user++] = *u;
    return 1;
}

int tambah_peminjaman_ke_array(const Peminjaman *p) {
    if (jumlah_pinjam >= kap_pinjam) {
        int kap_baru = (kap_pinjam == 0) ? KAPASITAS_AWAL : kap_pinjam * 2;
        Peminjaman *tmp = realloc(daftar_pinjam, kap_baru * sizeof(Peminjaman));
        if (tmp == NULL) {
            printf("Error: memori tidak cukup untuk data peminjaman.\n");
            return 0;
        }
        daftar_pinjam = tmp;
        kap_pinjam = kap_baru;
    }
    daftar_pinjam[jumlah_pinjam++] = *p;
    return 1;
}

//membaca data dari books.txt, memecah menjadi potongan informasi, lalu dimasukkan ke array dinamis daftar_buku
void muat_buku_dari_asli(void) {
    FILE *f = fopen(FILE_ASLI, "r");   
    char baris[PANJANG_BARIS];
    char tok_ocaid[64], tok_title[1280], tok_author[512];
    char tok_year[32], tok_page[32];

    kosongkan_buku(); //membersihkan data lama sebelum mengisi data baru 

    if (f == NULL) {
        printf("Error: file %s tidak ditemukan.\n", FILE_ASLI);
        return;
    }

    while (fgets(baris, sizeof(baris), f) != NULL) { //ini perulangannya per baris 
        char *p = baris;
        Buku b; //semua variabel struct buku dikumpulkan dalam b
        int year, page;

        buang_newline(baris); //buang newline
        if (baris_kosong(baris)) { //lewati baris kosong
            continue;
        }
        
        //mengambil 5 potongan data dari 1 baris: id, judul, penulis, tahun, dan halaman
        if (!ambil_token(&p, tok_ocaid, sizeof(tok_ocaid)) ||
            !ambil_token(&p, tok_title, sizeof(tok_title)) ||
            !ambil_token(&p, tok_author, sizeof(tok_author)) ||
            !ambil_token(&p, tok_year, sizeof(tok_year)) ||
            !ambil_token(&p, tok_page, sizeof(tok_page))) {
            continue;   
        }

        //mengecek dan mengubah data yang diambil menjadi integer (misal "2005")
        if (!ke_int(tok_year, &year) || !ke_int(tok_page, &page)) {
            continue;   
        }

        //menyalin semua potongan informasi ke dalam b
        //(variabel.bagian adalah cara untuk mengakses isi struct)
        salin(b.ocaid, tok_ocaid, sizeof(b.ocaid));
        salin(b.title, tok_title, sizeof(b.title));
        salin(b.author, tok_author, sizeof(b.author));
        b.year = year; //nilai tahun yang di return fungsi ke_int
        b.page_count = page; //nilai halaman dari ke_int
        b.stok = STOK_AWAL; //nilai awal stok sama = 3

        if (!tambah_buku_ke_array(&b)) { //kirim ke fungsi ini
            break;
        }
    }
    fclose(f);
}

//menyimpan data buku dari array (bisa dari fungsi lain) ke dalam file dengan format pemisah ;, mengecek jika ada ; dalam teks/data
void simpan_katalog(void) {
    FILE *f = fopen(FILE_KATALOG, "w");
    int i;
    if (f == NULL) {
        printf("Error: gagal menulis %s.\n", FILE_KATALOG);
        return;
    }
    for (i = 0; i < jumlah_buku; i++) {//satu putaran mencetak 1 buku ke file dari array daftar_buku yang isinya struct buku
        tulis_aman(f, daftar_buku[i].ocaid, ';');      fputc(';', f);
        tulis_aman(f, daftar_buku[i].title, ';');      fputc(';', f);
        tulis_aman(f, daftar_buku[i].author, ';');     fputc(';', f);
        fprintf(f, "%d;%d;%d\n",
                daftar_buku[i].year,
                daftar_buku[i].page_count,
                daftar_buku[i].stok);
    }
    fclose(f);
}

//mengisi data buku ke memori 
void muat_katalog(void) {
    FILE *f = fopen(FILE_KATALOG, "r");
    char baris[PANJANG_BARIS];

    if (f == NULL) { //kalo file belum ada, akan diisi dari books.txt,, menggunakan muat buku dari asli > array daftar buku > file katalog.txt 
       
        muat_buku_dari_asli();
        if (jumlah_buku == 0) {
          
            printf("Error: tidak ada buku yang terbaca dari %s, %s tidak dibuat.\n",
                   FILE_ASLI, FILE_KATALOG);
            return;
        }
        simpan_katalog();
        return;
    }
    //kosongkan array daftar buku
    kosongkan_buku();

    //kalo file katalog.txt udah ada, maka kita akan mengisi struct buku dengan data dari katalog.txt, lalu 1 struct perloop akan diisi ke array daftar_buku 
    while (fgets(baris, sizeof(baris), f) != NULL) {
        char *kolom[6];
        Buku b;
        int year, page, stok;

        buang_newline(baris);
        if (baris_kosong(baris)) {
            continue;
        }
        if (pecah(baris, ';', kolom, 6) != 6) {
            continue;  
        }
        if (!ke_int(kolom[3], &year) || //mengecek apakah integer lalu diberikan ke variabel year
            !ke_int(kolom[4], &page) ||
            !ke_int(kolom[5], &stok)) {
            continue;
        }

        salin(b.ocaid, kolom[0], sizeof(b.ocaid)); //menyalin data dari file ke array dinamis
        salin(b.title, kolom[1], sizeof(b.title));
        salin(b.author, kolom[2], sizeof(b.author));
        b.year = year;
        b.page_count = page;
        b.stok = stok;

        if (!tambah_buku_ke_array(&b)) { //mengisi 1 struct per loop ke array daftar_buku, kalo gagal (misal karena memori penuh), loop dihentikan
            break;
        }
    }
    fclose(f);
}

void muat_user(void) {
    FILE *f = fopen(FILE_USER, "r");
    char baris[PANJANG_BARIS];

    kosongkan_user();

    if (f == NULL) {
        printf("Error: file %s tidak ditemukan.\n", FILE_USER);
        return;
    }

    while (fgets(baris, sizeof(baris), f) != NULL) {
        char *kolom[3];
        User u; //variabel u yang menampung tiap variabel dalam struct user

        buang_newline(baris);
        if (baris_kosong(baris)) {
            continue;
        }
        if (pecah(baris, ',', kolom, 3) != 3) {
            continue;
        }

        //menyalin data dari kolom yang dipecah ke variabel bagian struct u
        salin(u.username, kolom[0], sizeof(u.username));
        salin(u.password, kolom[1], sizeof(u.password));
        salin(u.role, kolom[2], sizeof(u.role));

        if (!tambah_user_ke_array(&u)) { //setelah menjadi 1 struct, keseluruhan 1 struct per loop akan diisi ke array daftar_user
            break;
        }
    }
    fclose(f);
}

void simpan_user(void) {
    FILE *f = fopen(FILE_USER, "w");
    int i;
    if (f == NULL) {
        printf("Error: gagal menulis %s.\n", FILE_USER);
        return;
    }
    for (i = 0; i < jumlah_user; i++) {
        tulis_aman(f, daftar_user[i].username, ','); fputc(',', f);
        tulis_aman(f, daftar_user[i].password, ','); fputc(',', f);
        tulis_aman(f, daftar_user[i].role, ',');     fputc('\n', f);
    }
    fclose(f);
}

void muat_peminjaman(void) {
    FILE *f = fopen(FILE_PEMINJAMAN, "r");
    char baris[PANJANG_BARIS];

    kosongkan_peminjaman();

    if (f == NULL) {
        return;
    }

    while (fgets(baris, sizeof(baris), f) != NULL) {
        char *kolom[5];
        Peminjaman p; //variabel p yang menampung tiap variabel dalam struct peminjaman
        int id;

        buang_newline(baris);
        if (baris_kosong(baris)) {
            continue;
        }
        if (pecah(baris, ';', kolom, 5) != 5) {
            continue;
        }
        if (!ke_int(kolom[0], &id) || id < 0) {
            continue;
        }

        p.id_peminjaman = (unsigned int)id; //mendapat nomor id peminjaman dari struct peminjaman bagian id
        salin(p.username, kolom[1], sizeof(p.username));
        salin(p.ocaid, kolom[2], sizeof(p.ocaid));
        salin(p.tanggal_pinjam, kolom[3], sizeof(p.tanggal_pinjam));
        salin(p.status, kolom[4], sizeof(p.status));

        if (!tambah_peminjaman_ke_array(&p)) { //setelah struct terisi maka akan dimasukkan ke array daftar_pinjaman
            break;
        }
    }
    fclose(f);
}

//mengisi file peminjaman.txt dengan array daftar_pinjaman yang diisi dari fungsi lain nantinya
void simpan_peminjaman(void) {
    FILE *f = fopen(FILE_PEMINJAMAN, "w");
    int i;
    if (f == NULL) {
        printf("Error: gagal menulis %s.\n", FILE_PEMINJAMAN);
        return;
    }
    for (i = 0; i < jumlah_pinjam; i++) {
        fprintf(f, "%u;", daftar_pinjam[i].id_peminjaman); //mengisi file dari isi array daftar_pinjaman yang isinya struct peminjaman, dengan pemisah ;
        tulis_aman(f, daftar_pinjam[i].username, ';');       fputc(';', f);
        tulis_aman(f, daftar_pinjam[i].ocaid, ';');          fputc(';', f);
        tulis_aman(f, daftar_pinjam[i].tanggal_pinjam, ';'); fputc(';', f);
        tulis_aman(f, daftar_pinjam[i].status, ';');         fputc('\n', f);
    }
    fclose(f);
}

//di panggil di akhir main(), untuk membersihkan memori saat perogram akan ditutup, untuk mencegah memory leak
void bebaskan_semua(void) {
    kosongkan_buku();
    kosongkan_user();
    kosongkan_peminjaman();
}
