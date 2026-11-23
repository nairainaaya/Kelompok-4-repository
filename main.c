#include <stdio.h>
#include <string.h>
#include "data.h"
#include "auth.h"
#include "buku.h"
#include "sorting.h"
#include "transaksi.h"
#include "input.h"

static void menu_cari(void)
{
    int pilihan;

    printf("\n--- CARI BUKU ---\n");
    printf("1. Berdasarkan ID\n");
    printf("2. Berdasarkan judul\n");
    printf("0. Kembali\n");

    pilihan = baca_int("Pilih (0-2): ", 0, 2);
    if (pilihan == 1) {
        cari_buku_by_id();
    } else if (pilihan == 2) {
        cari_buku_by_judul();
    }
}

static void menu_admin(const char *username)
{
    int pilihan;

    do {
        printf("\n=== MENU ADMIN (%s) ===\n", username);
        printf("1. Tampilkan Katalog Buku\n");
        printf("2. Urutkan Katalog (Sorting)\n");
        printf("3. Cari Buku\n");
        printf("4. Tambah Buku\n");
        printf("5. Ubah Data Buku\n");
        printf("6. Hapus Buku\n");
        printf("7. Laporan Transaksi Peminjaman\n");
        printf("0. Logout\n");

        pilihan = baca_int("Pilih menu (0-7): ", 0, 7);

        if (feof(stdin)) {
            printf("\nInput dihentikan.\n");
            return;
        }

        switch (pilihan) {
            case 1:
                tampil_katalog(0);
                break;
            case 2:
                menu_sorting();
                break;
            case 3:
                menu_cari();
                break;
            case 4:
                tambah_buku();
                break;
            case 5:
                ubah_buku();
                break;
            case 6:
                hapus_buku();   
                break;
            case 7:
                tampil_laporan_transaksi();
                break;
            case 0:
                if (konfirmasi("Apakah Anda yakin ingin logout?")) {
                    printf("Logout berhasil.\n");
                } else {
                    pilihan = -1;   
                }
                break;
        }
    } while (pilihan != 0);
}

static void menu_user(const char *username)
{
    int pilihan;

    do {
        printf("\n=== MENU USER (%s) ===\n", username);
        printf("1. Lihat Katalog Buku (yang tersedia)\n");
        printf("2. Urutkan Katalog (Sorting)\n");
        printf("3. Cari Buku\n");
        printf("4. Pinjam Buku\n");
        printf("5. Kembalikan Buku\n");
        printf("6. Buku yang Sedang Saya Pinjam\n");
        printf("0. Logout\n");

        pilihan = baca_int("Pilih menu (0-6): ", 0, 6);

        if (feof(stdin)) {
            printf("\nInput dihentikan.\n");
            return;
        }

        switch (pilihan) {
            case 1:
                tampil_katalog(1);
                break;
            case 2:
                menu_sorting_tersedia();
                break;
            case 3:
                menu_cari();
                break;
            case 4:
                pinjam_buku(username);
                break;
            case 5:
                kembalikan_buku(username);
                break;
            case 6:
                tampil_pinjaman_saya(username);
                break;
            case 0:
                if (konfirmasi("Apakah Anda yakin ingin logout?")) {
                    printf("Logout berhasil.\n");
                } else {
                    pilihan = -1;
                }
                break;
        }
    } while (pilihan != 0);
}

int main(void)
{
    char username_aktif[32];
    const char *role;
    int ulangi = 1;

    muat_katalog();
    muat_user();
    muat_peminjaman();

    if (jumlah_user == 0) {
        printf("Peringatan: data user kosong. "
               "Pastikan file user.txt ada di folder program!\n");
        bebaskan_semua();
        return 1;
    }
    if (jumlah_buku == 0) {
        printf("Peringatan: katalog buku kosong. "
               "Pastikan Books.txt ada di folder program.\n");
    }

    printf("=== SISTEM MANAJEMEN PERPUSTAKAAN ===\n");

    while (ulangi) {
        role = login(daftar_user, jumlah_user,
                     username_aktif, sizeof(username_aktif));
        if (role == NULL) {
            break;   
        }

        if (strcmp(role, "admin") == 0) {
            menu_admin(username_aktif);
        } else if (strcmp(role, "user") == 0) {
            menu_user(username_aktif);
        } else {
            printf("Error: role '%s' tidak dikenali sistem.\n", role);
        }

        if (feof(stdin)) {
            break;
        }
        ulangi = konfirmasi("Login dengan akun lain?");
    }

    if (jumlah_buku > 0) {
        simpan_katalog();
    }
    simpan_peminjaman();
    bebaskan_semua();

    printf("Program selesai. Terima kasih!\n");
    return 0;
}
