#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "buku.h"
#include "input.h"

#define PER_HALAMAN  20
#define LEBAR_TABEL  125 
#define LEBAR_ID      30
#define LEBAR_JUDUL   50
#define LEBAR_PENULIS 20

#define TAHUN_MIN    1
#define TAHUN_MAX    2100
#define HALAMAN_MIN  1
#define HALAMAN_MAX  100000
#define STOK_MIN     0
#define STOK_MAX     1000

static void garis(void) {
    int i;
    for (i = 0; i < LEBAR_TABEL; i++) {
        putchar('-');
    }
    putchar('\n');
}

static void tampil_header(void) {
    printf("%-5s %-30s %-50s %-20s %-5s %-5s %-4s\n",
           "No", "ID", "Judul", "Penulis", "Tahun", "Hal", "Stok");
    garis();
}

static size_t ambil_bagian(const char *s, size_t lebar) {
    size_t n = strlen(s);
    return (n > lebar) ? lebar : n;
}


static void tampil_baris(int no, const Buku *b) {
    const char *jdl = b->title;
    const char *pnl = b->author;
    size_t nj, np;
    int pertama = 1;

    do {
        nj = ambil_bagian(jdl, LEBAR_JUDUL);
        np = ambil_bagian(pnl, LEBAR_PENULIS);

        if (pertama) {
            printf("%-5d %-*.*s %-*.*s %-*.*s %-5d %-5d %-4d\n",
                   no,
                   LEBAR_ID, LEBAR_ID, b->ocaid,
                   LEBAR_JUDUL, (int)nj, jdl,
                   LEBAR_PENULIS, (int)np, pnl,
                   b->year, b->page_count, b->stok);
            pertama = 0;
        } else {
            printf("%-5s %-*s %-*.*s %-*.*s\n",
                   "", LEBAR_ID, "",
                   LEBAR_JUDUL, (int)nj, jdl,
                   LEBAR_PENULIS, (int)np, pnl);
        }

        jdl += nj;
        pnl += np;
    } while (*jdl != '\0' || *pnl != '\0');
}

static int teks_valid(const char *s, int boleh_spasi) {
    if (s[0] == '\0') {
        return 0;
    }
    for (; *s != '\0'; s++) {
        if (*s == ';') {
            return 0;
        }
        if (!boleh_spasi && isspace((unsigned char)*s)) {
            return 0;
        }
    }
    return 1;
}

static void salin_teks(char *tujuan, size_t ukuran, const char *sumber) {
    strncpy(tujuan, sumber, ukuran - 1);
    tujuan[ukuran - 1] = '\0';
}

static int baca_teks(const char *prompt, char *buf, int ukuran,
                     int boleh_spasi) {
    while (1) {
        baca_string(prompt, buf, ukuran);
        if (teks_valid(buf, boleh_spasi)) {
            return 1;
        }
        if (feof(stdin)) {
            return 0;
        }
        if (boleh_spasi) {
            printf("Input tidak boleh kosong atau mengandung ';'. Coba lagi.\n");
        } else {
            printf("Input tidak boleh kosong, berisi spasi, atau ';'. Coba lagi.\n");
        }
    }
}

static void ke_huruf_kecil(const char *src, char *dst, size_t ukuran) {
    size_t i;
    for (i = 0; i + 1 < ukuran && src[i] != '\0'; i++) {
        dst[i] = (char)tolower((unsigned char)src[i]);
    }
    dst[i] = '\0';
}

static void tampil_berhalaman(const Buku arr[], const int indeks[],
                              int jumlah) {
    int halaman = 0;
    int total_halaman = (jumlah + PER_HALAMAN - 1) / PER_HALAMAN;
    int i, awal, akhir, c, lompat, selesai = 0;
    char cmd[16];
    char *akhir_angka;

    while (!selesai) {
        awal = halaman * PER_HALAMAN;
        akhir = awal + PER_HALAMAN;
        if (akhir > jumlah) {
            akhir = jumlah;
        }

        printf("\n");
        tampil_header();
        for (i = awal; i < akhir; i++) {
            tampil_baris(i + 1, &arr[indeks[i]]);
        }
        garis();
        printf("Halaman %d/%d | Buku %d-%d dari %d\n",
               halaman + 1, total_halaman, awal + 1, akhir, jumlah);

        /* Satu halaman saja: tidak perlu navigasi */
        if (total_halaman <= 1) {
            return;
        }

        while (1) {
            baca_string("[n] berikut  [p] sebelumnya  [angka] ke halaman  [q] keluar: ",
                        cmd, sizeof(cmd));
            if (cmd[0] == '\0' && feof(stdin)) {   /* input berakhir */
                selesai = 1;
                break;
            }
            c = tolower((unsigned char)cmd[0]);

            if (isdigit((unsigned char)cmd[0])) {
                lompat = (int)strtol(cmd, &akhir_angka, 10);
                if (*akhir_angka == '\0' && lompat >= 1 && lompat <= total_halaman) {
                    halaman = lompat - 1;
                    break;
                }
            } else if (c == 'n' && halaman < total_halaman - 1) {
                halaman++;
                break;
            } else if (c == 'p' && halaman > 0) {
                halaman--;
                break;
            } else if (c == 'q') {
                selesai = 1;
                break;
            }
            printf("Pilihan tidak tersedia di halaman ini.\n");
        }
    }
}

int cari_index_buku(const char *ocaid) {
    int i;
    for (i = 0; i < jumlah_buku; i++) {
        if (strcmp(ocaid, daftar_buku[i].ocaid) == 0) {
            return i;
        }
    }
    return -1;
}

void cari_buku_by_id(void) {
    char id[64];
    int idx;

    printf("\n===== CARI BUKU BERDASARKAN ID =====\n");
    baca_string("Masukkan ID buku: ", id, sizeof(id));

    idx = cari_index_buku(id);
    if (idx < 0) {
        printf("Buku dengan ID '%s' tidak ditemukan.\n", id);
        return;
    }

    printf("\n");
    tampil_header();
    tampil_baris(1, &daftar_buku[idx]);
    garis();
}

void cari_buku_by_judul(void) {
    char kata[256];
    char kata_kecil[256];
    char judul_kecil[1280];
    int *hasil;
    int i, ditemukan = 0;

    printf("\n===== CARI BUKU BERDASARKAN JUDUL =====\n");
    baca_string("Masukkan judul (boleh sebagian kata): ", kata, sizeof(kata));

    if (kata[0] == '\0') {
        printf("Kata kunci tidak boleh kosong.\n");
        return;
    }
    if (jumlah_buku == 0) {
        printf("Belum ada buku di katalog.\n");
        return;
    }

    hasil = (int *)malloc((size_t)jumlah_buku * sizeof(int));
    if (hasil == NULL) {
        printf("Memori tidak cukup untuk pencarian.\n");
        return;
    }

    ke_huruf_kecil(kata, kata_kecil, sizeof(kata_kecil));

    for (i = 0; i < jumlah_buku; i++) {
        ke_huruf_kecil(daftar_buku[i].title, judul_kecil, sizeof(judul_kecil));
        if (strstr(judul_kecil, kata_kecil) != NULL) {
            hasil[ditemukan++] = i;
        }
    }

    if (ditemukan == 0) {
        printf("Tidak ada buku dengan judul mengandung '%s'.\n", kata);
    } else {
        printf("Ditemukan %d buku.\n", ditemukan);
        tampil_berhalaman(daftar_buku, hasil, ditemukan);
    }

    free(hasil);
}

void tampil_tabel_buku(const Buku arr[], int n, int hanya_tersedia) {
    int *indeks;
    int i, jumlah_tampil = 0;

    if (n <= 0) {
        printf("\nBelum ada buku di katalog.\n");
        return;
    }

    indeks = (int *)malloc((size_t)n * sizeof(int));
    if (indeks == NULL) {
        printf("Memori tidak cukup untuk menampilkan katalog.\n");
        return;
    }

    for (i = 0; i < n; i++) {
        if (!hanya_tersedia || arr[i].stok > 0) {
            indeks[jumlah_tampil++] = i;
        }
    }

    if (jumlah_tampil == 0) {
        printf("\nTidak ada buku yang tersedia saat ini.\n");
    } else {
        tampil_berhalaman(arr, indeks, jumlah_tampil);
    }

    free(indeks);
}

void tampil_katalog(int hanya_tersedia) {
    if (hanya_tersedia) {
        printf("\n===== KATALOG BUKU TERSEDIA =====\n");
    } else {
        printf("\n===== KATALOG BUKU =====\n");
    }
    tampil_tabel_buku(daftar_buku, jumlah_buku, hanya_tersedia);
}

void tambah_buku(void) {
    Buku b;

    memset(&b, 0, sizeof(b));

    printf("\n===== TAMBAH BUKU =====\n");

    if (!baca_teks("ID buku (tanpa spasi): ", b.ocaid, (int)sizeof(b.ocaid), 0)) {
        printf("\nInput dihentikan, buku tidak ditambahkan.\n");
        return;
    }

    if (cari_index_buku(b.ocaid) >= 0) {
        printf("Gagal: ID '%s' sudah dipakai buku lain.\n", b.ocaid);
        return;
    }

    if (!baca_teks("Judul: ",    b.title,     (int)sizeof(b.title),     1) ||
        !baca_teks("Penulis: ",  b.author,    (int)sizeof(b.author),    1)) {
        printf("\nInput dihentikan, buku tidak ditambahkan.\n");
        return;
    }

    b.year       = baca_int("Tahun terbit: ",    TAHUN_MIN,   TAHUN_MAX);
    b.page_count = baca_int("Jumlah halaman: ",  HALAMAN_MIN, HALAMAN_MAX);
    b.stok       = baca_int("Stok: ",            STOK_MIN,    STOK_MAX);

    if (feof(stdin)) {
        printf("\nInput dihentikan, buku tidak ditambahkan.\n");
        return;
    }

    if (tambah_buku_ke_array(&b)) {
        simpan_katalog();
        printf("Buku berhasil ditambahkan!\n");
    } else {
        printf("Buku gagal ditambahkan (kapasitas penuh?).\n");
    }
}


void ubah_buku(void) {
    char id[64];
    char tmp[1280];   
    int idx, pilihan, nilai, berubah = 0;
    Buku *b;

    printf("\n===== UBAH BUKU =====\n");
    baca_string("Masukkan ID buku yang ingin diubah: ", id, sizeof(id));

    idx = cari_index_buku(id);
    if (idx < 0) {
        printf("Buku tidak ditemukan!\n");
        return;
    }

    b = &daftar_buku[idx];

    do {
        printf("\nData saat ini:\n");
        tampil_header();
        tampil_baris(1, b);
        garis();

        printf("\nPilih atribut yang ingin diubah:\n");
        printf("1. Judul\n");
        printf("2. Penulis\n");
        printf("3. Tahun terbit\n");
        printf("4. Jumlah halaman\n");
        printf("5. Stok\n");
        printf("0. Selesai\n");

        pilihan = baca_int("Pilihan: ", 0, 5);

        switch (pilihan) {
            case 1:
                if (!baca_teks("Judul baru: ", tmp, (int)sizeof(b->title), 1)) {
                    pilihan = 0;   /* input berakhir: keluar tanpa mengubah */
                    break;
                }
                salin_teks(b->title, sizeof(b->title), tmp);
                berubah = 1;
                break;
            case 2:
                if (!baca_teks("Penulis baru: ", tmp, (int)sizeof(b->author), 1)) {
                    pilihan = 0;
                    break;
                }
                salin_teks(b->author, sizeof(b->author), tmp);
                berubah = 1;
                break;
            case 3:
                nilai = baca_int("Tahun baru: ", TAHUN_MIN, TAHUN_MAX);
                if (feof(stdin)) {
                    pilihan = 0;
                    break;
                }
                b->year = nilai;
                berubah = 1;
                break;
            case 4:
                nilai = baca_int("Jumlah halaman baru: ",
                                 HALAMAN_MIN, HALAMAN_MAX);
                if (feof(stdin)) {
                    pilihan = 0;
                    break;
                }
                b->page_count = nilai;
                berubah = 1;
                break;
            case 5:
                nilai = baca_int("Stok baru: ", STOK_MIN, STOK_MAX);
                if (feof(stdin)) {
                    pilihan = 0;
                    break;
                }
                b->stok = nilai;
                berubah = 1;
                break;
            default:
                break;
        }
    } while (pilihan != 0);

    if (berubah) {
        simpan_katalog();
        printf("Data berhasil diubah dan disimpan!\n");
    } else {
        printf("Tidak ada perubahan.\n");
    }
}

void hapus_buku(void) {
    char id[64];
    char prompt[320];
    int idx, j;

    printf("\n===== HAPUS BUKU =====\n");
    baca_string("Masukkan ID buku yang ingin dihapus: ", id, sizeof(id));

    idx = cari_index_buku(id);
    if (idx < 0) {
        printf("Buku tidak ditemukan!\n");
        return;
    }

    sprintf(prompt, "Yakin menghapus buku '%.200s'?",
            daftar_buku[idx].title);
    if (!konfirmasi(prompt)) {
        printf("Penghapusan dibatalkan.\n");
        return;
    }

    for (j = idx; j < jumlah_buku - 1; j++) {
        daftar_buku[j] = daftar_buku[j + 1];
    }
    jumlah_buku--;

    simpan_katalog();
    printf("Buku berhasil dihapus!\n");
}