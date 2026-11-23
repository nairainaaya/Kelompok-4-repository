#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "transaksi.h"
#include "buku.h"
#include "input.h"

#define BARIS_PER_HALAMAN 20

static void salin(char *tujuan, size_t ukuran, const char *sumber) {
    strncpy(tujuan, sumber, ukuran - 1);
    tujuan[ukuran - 1] = '\0';
}

void ambil_tanggal_hari_ini(char *out) {
    time_t sekarang = time(NULL);
    struct tm *info = localtime(&sekarang);

    if (info == NULL || strftime(out, 11, "%d-%m-%Y", info) == 0) {
        strcpy(out, "-");
    }
}

static const char *judul_buku(const char *ocaid) {
    int idx = cari_index_buku(ocaid);

    if (idx < 0) {
        return "(buku sudah dihapus)";
    }
    return daftar_buku[idx].title;
}

static void garis(int panjang) {
    int i;
    for (i = 0; i < panjang; i++) {
        putchar('-');
    }
    putchar('\n');
}

static int tampil_aktif_user(const char *username) {
    int i, jumlah = 0;

    for (i = 0; i < jumlah_pinjam; i++) {
        if (strcmp(daftar_pinjam[i].username, username) == 0 &&
            strcmp(daftar_pinjam[i].status, STATUS_DIPINJAM) == 0) {

            if (jumlah == 0) {
                printf("\n%-7s %-26s %-28s %-10s\n",
                       "ID Trx", "ID Buku", "Judul Buku", "Tgl Pinjam");
                garis(74);
            }
            printf("%-7u %-26.26s %-28.28s %-10s\n",
                   daftar_pinjam[i].id_peminjaman,
                   daftar_pinjam[i].ocaid,
                   judul_buku(daftar_pinjam[i].ocaid),
                   daftar_pinjam[i].tanggal_pinjam);
            jumlah++;
        }
    }

    if (jumlah == 0) {
        printf("Kamu tidak sedang meminjam buku apa pun.\n");
    } else {
        garis(74);
    }
    return jumlah;
}

static void jeda(void) {
    char tmp[8];
    baca_string("\nTekan Enter untuk kembali ke menu...", tmp, sizeof(tmp));
}

unsigned int buat_id_transaksi(void) {
    unsigned int maks = 0;
    int i;

    for (i = 0; i < jumlah_pinjam; i++) {
        if (daftar_pinjam[i].id_peminjaman > maks) {
            maks = daftar_pinjam[i].id_peminjaman;
        }
    }
    return maks + 1;
}


void pinjam_buku(const char *username) {
    char id[64];
    Peminjaman p;
    int i, idx, tersedia = 0;

    if (username == NULL || username[0] == '\0') {
        printf("Error: username tidak valid.\n");
        return;
    }

    printf("\n===== PEMINJAMAN BUKU =====\n");

    for (i = 0; i < jumlah_buku; i++) {
        if (daftar_buku[i].stok > 0) {
            tersedia = 1;
            break;
        }
    }
    if (!tersedia) {
        printf("Maaf, tidak ada buku yang tersedia untuk dipinjam.\n");
        return;
    }

    tampil_katalog(1);

    baca_string("\nMasukkan ID buku yang ingin dipinjam (0 untuk batal): ",
                id, sizeof(id));

    if (id[0] == '\0' || strcmp(id, "0") == 0) {
        printf("Peminjaman dibatalkan.\n");
        return;
    }

    idx = cari_index_buku(id);
    if (idx < 0) {
        printf("ID buku '%s' tidak ditemukan!\n", id);
        return;
    }
    if (daftar_buku[idx].stok <= 0) {
        printf("Maaf, stok buku ini sudah habis.\n");
        return;
    }

    for (i = 0; i < jumlah_pinjam; i++) {
        if (strcmp(daftar_pinjam[i].username, username) == 0 &&
            strcmp(daftar_pinjam[i].ocaid, daftar_buku[idx].ocaid) == 0 &&
            strcmp(daftar_pinjam[i].status, STATUS_DIPINJAM) == 0) {
            printf("Kamu masih meminjam buku ini dan belum mengembalikannya!\n");
            return;
        }
    }

    memset(&p, 0, sizeof(p));
    p.id_peminjaman = buat_id_transaksi();
    salin(p.username, sizeof(p.username), username);
    salin(p.ocaid, sizeof(p.ocaid), daftar_buku[idx].ocaid);
    ambil_tanggal_hari_ini(p.tanggal_pinjam);
    salin(p.status, sizeof(p.status), STATUS_DIPINJAM);

    if (!tambah_peminjaman_ke_array(&p)) {
        printf("Peminjaman gagal disimpan.\n");
        return;
    }

    daftar_buku[idx].stok--;
    simpan_katalog();
    simpan_peminjaman();

    printf("\nBerhasil meminjam buku '%s'.\n", daftar_buku[idx].title);
    printf("ID Transaksi: %u | Tanggal pinjam: %s\n",
           p.id_peminjaman, p.tanggal_pinjam);
}

void kembalikan_buku(const char *username) {
    int i, idx_trx = -1, idx_buku;
    int nilai;
    unsigned int id_trx;

    if (username == NULL || username[0] == '\0') {
        printf("Error: username tidak valid.\n");
        return;
    }

    printf("\n===== PENGEMBALIAN BUKU =====\n");

    if (tampil_aktif_user(username) == 0) {
        jeda();
        return;
    }


    nilai = baca_int("\nMasukkan ID transaksi yang ingin dikembalikan (0 untuk batal): ",
                     0, INT_MAX);
    if (nilai == 0) {
        printf("Pengembalian dibatalkan.\n");
        return;
    }
    id_trx = (unsigned int)nilai;

    for (i = 0; i < jumlah_pinjam; i++) {
        if (daftar_pinjam[i].id_peminjaman == id_trx &&
            strcmp(daftar_pinjam[i].username, username) == 0 &&
            strcmp(daftar_pinjam[i].status, STATUS_DIPINJAM) == 0) {
            idx_trx = i;
            break;
        }
    }
    if (idx_trx < 0) {
        printf("ID transaksi tidak ditemukan atau sudah dikembalikan!\n");
        return;
    }

    salin(daftar_pinjam[idx_trx].status,
          sizeof(daftar_pinjam[idx_trx].status), STATUS_KEMBALI);

    idx_buku = cari_index_buku(daftar_pinjam[idx_trx].ocaid);
    if (idx_buku >= 0) {
        daftar_buku[idx_buku].stok++;
        simpan_katalog();
    }
    simpan_peminjaman();

    if (idx_buku >= 0) {
        printf("\nBuku '%s' berhasil dikembalikan!\n",
               daftar_buku[idx_buku].title);
    } else {
        printf("\nBuku berhasil dikembalikan! "
               "(buku ini sudah tidak ada di katalog)\n");
    }
}


void tampil_pinjaman_saya(const char *username) {
    if (username == NULL || username[0] == '\0') {
        printf("Error: username tidak valid.\n");
        return;
    }

    printf("\n===== BUKU YANG SEDANG KAMU PINJAM =====\n");
    tampil_aktif_user(username);
    jeda();
}

void tampil_laporan_transaksi(void) {
    int i, dipinjam = 0, kembali = 0;
    char cmd[16];

    printf("\n===== LAPORAN TRANSAKSI PEMINJAMAN =====\n");

    if (jumlah_pinjam == 0) {
        printf("Belum ada transaksi peminjaman.\n");
        jeda();
        return;
    }

    for (i = 0; i < jumlah_pinjam; i++) {
        if (strcmp(daftar_pinjam[i].status, STATUS_DIPINJAM) == 0) {
            dipinjam++;
        } else {
            kembali++;
        }
    }

    printf("\n%-7s %-12s %-26s %-24s %-10s %-9s\n",
           "ID Trx", "User", "ID Buku", "Judul Buku", "Tgl Pinjam", "Status");
    garis(94);

    for (i = 0; i < jumlah_pinjam; i++) {
        printf("%-7u %-12.12s %-26.26s %-24.24s %-10s %-9s\n",
               daftar_pinjam[i].id_peminjaman,
               daftar_pinjam[i].username,
               daftar_pinjam[i].ocaid,
               judul_buku(daftar_pinjam[i].ocaid),
               daftar_pinjam[i].tanggal_pinjam,
               daftar_pinjam[i].status);

        /* Jeda per halaman jika transaksinya banyak */
        if ((i + 1) % BARIS_PER_HALAMAN == 0 && i + 1 < jumlah_pinjam) {
            baca_string("-- Enter: lanjut, q: berhenti -- ", cmd, sizeof(cmd));
            if (cmd[0] == 'q' || cmd[0] == 'Q') {
                break;
            }
            if (cmd[0] == '\0' && feof(stdin)) {  
                break;
            }
        }
    }

    garis(94);
    printf("Total transaksi: %d | Sedang dipinjam: %d | Dikembalikan: %d\n",
           jumlah_pinjam, dipinjam, kembali);
}
