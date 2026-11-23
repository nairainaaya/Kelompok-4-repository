#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "sorting.h"
#include "buku.h"
#include "input.h"

void bubble_sort(Buku arr[], int n,
                 int (*bandingkan)(const Buku *, const Buku *)) {
    int i, j, tukar;
    Buku temp;

    if (arr == NULL || bandingkan == NULL || n < 2) {
        return;
    }

    for (i = 0; i < n - 1; i++) {
        tukar = 0;
        for (j = 0; j < n - i - 1; j++) {
            if (bandingkan(&arr[j], &arr[j + 1]) > 0) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                tukar = 1;
            }
        }
        if (!tukar) {
            break;
        }
    }
}

void insertion_sort(Buku arr[], int n,
                    int (*bandingkan)(const Buku *, const Buku *)) {
    int i, j;
    Buku kunci;

    if (arr == NULL || bandingkan == NULL || n < 2) {
        return;
    }

    for (i = 1; i < n; i++) {
        kunci = arr[i];
        j = i - 1;
        while (j >= 0 && bandingkan(&arr[j], &kunci) > 0) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = kunci;
    }
}

void selection_sort(Buku arr[], int n,
                    int (*bandingkan)(const Buku *, const Buku *)) {
    int i, j, idx_awal;
    Buku temp;

    if (arr == NULL || bandingkan == NULL || n < 2) {
        return;
    }

    for (i = 0; i < n - 1; i++) {
        idx_awal = i;
        for (j = i + 1; j < n; j++) {
            if (bandingkan(&arr[idx_awal], &arr[j]) > 0) {
                idx_awal = j;
            }
        }
        if (idx_awal != i) {
            temp = arr[i];
            arr[i] = arr[idx_awal];
            arr[idx_awal] = temp;
        }
    }
}

static int banding_teks(const char *a, const char *b) {
    while (*a != '\0' && *b != '\0') {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) {
            return ca - cb;
        }
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

static int banding_angka(int a, int b) {
    return (a > b) - (a < b);
}

static int atau_id(int hasil, const Buku *a, const Buku *b) {
    if (hasil != 0) {
        return hasil;
    }
    return strcmp(a->ocaid, b->ocaid);
}

int banding_id_asc(const Buku *a, const Buku *b) {
    return strcmp(a->ocaid, b->ocaid);
}

int banding_id_desc(const Buku *a, const Buku *b) {
    return -banding_id_asc(a, b);
}

int banding_judul_asc(const Buku *a, const Buku *b) {
    return atau_id(banding_teks(a->title, b->title), a, b);
}

int banding_judul_desc(const Buku *a, const Buku *b) {
    return -banding_judul_asc(a, b);
}

int banding_tahun_asc(const Buku *a, const Buku *b) {
    return atau_id(banding_angka(a->year, b->year), a, b);
}

int banding_tahun_desc(const Buku *a, const Buku *b) {
    return -banding_tahun_asc(a, b);
}

int banding_halaman(const Buku *a, const Buku *b) {
    return atau_id(banding_angka(a->page_count, b->page_count), a, b);
}

int banding_halaman_desc(const Buku *a, const Buku *b) {
    return -banding_halaman(a, b);
}

int banding_stok(const Buku *a, const Buku *b) {
    return atau_id(banding_angka(a->stok, b->stok), a, b);
}

int banding_stok_desc(const Buku *a, const Buku *b) {
    return -banding_stok(a, b);
}

typedef int (*Pembanding)(const Buku *, const Buku *);
typedef void (*Pengurut)(Buku[], int, Pembanding);

static Pembanding pilih_pembanding(int atribut, int arah) {
    switch (atribut) {
        case 1: return arah == 1 ? banding_id_asc     : banding_id_desc;
        case 2: return arah == 1 ? banding_judul_asc  : banding_judul_desc;
        case 3: return arah == 1 ? banding_tahun_asc  : banding_tahun_desc;
        case 4: return arah == 1 ? banding_halaman    : banding_halaman_desc;
        case 5: return arah == 1 ? banding_stok       : banding_stok_desc;
        default: return NULL;
    }
}

static Pengurut pilih_pengurut(int algoritma) {
    switch (algoritma) {
        case 1: return bubble_sort;
        case 2: return insertion_sort;
        case 3: return selection_sort;
        default: return NULL;
    }
}

static void jalankan_menu_sorting(int hanya_tersedia) {
    int algoritma, atribut, arah, i, tersedia = 0;
    Pembanding pembanding;
    Pengurut pengurut;
    Buku *salinan;
    clock_t mulai;
    double detik;

    if (jumlah_buku <= 0) {
        printf("\nData buku masih kosong!\n");
        return;
    }

    if (hanya_tersedia) {
        for (i = 0; i < jumlah_buku; i++) {
            if (daftar_buku[i].stok > 0) {
                tersedia = 1;
                break;
            }
        }
        if (!tersedia) {
            printf("\nTidak ada buku yang tersedia saat ini.\n");
            return;
        }
    }

    printf("\n===== MENU SORTING BUKU =====\n");
    printf("Pilih algoritma:\n");
    printf("1. Bubble Sort\n");
    printf("2. Insertion Sort\n");
    printf("3. Selection Sort\n");
    algoritma = baca_int("Pilihan algoritma (1-3): ", 1, 3);

    printf("\nUrutkan berdasarkan:\n");
    printf("1. ID Buku\n");
    printf("2. Judul Buku\n");
    printf("3. Tahun Terbit\n");
    printf("4. Jumlah Halaman\n");
    printf("5. Stok\n");
    atribut = baca_int("Pilihan atribut (1-5): ", 1, 5);

    printf("\nArah pengurutan:\n");
    printf("1. Naik (A-Z / terkecil - terbesar / terlama - terbaru)\n");
    printf("2. Turun (Z-A / terbesar - terkecil / terbaru - terlama)\n");
    arah = baca_int("Pilihan arah (1-2): ", 1, 2);

    if (feof(stdin)) {
        printf("\nInput dihentikan, pengurutan dibatalkan.\n");
        return;
    }

    pengurut = pilih_pengurut(algoritma);
    pembanding = pilih_pembanding(atribut, arah);
    if (pengurut == NULL || pembanding == NULL) {
        printf("Pilihan tidak valid!\n");
        return;
    }

    salinan = (Buku *)malloc((size_t)jumlah_buku * sizeof(Buku));
    if (salinan == NULL) {
        printf("Memori tidak cukup untuk mengurutkan katalog.\n");
        return;
    }
    memcpy(salinan, daftar_buku, (size_t)jumlah_buku * sizeof(Buku));

    printf("\nSedang mengurutkan %d buku...\n", jumlah_buku);
    mulai = clock();
    pengurut(salinan, jumlah_buku, pembanding);
    detik = (double)(clock() - mulai) / CLOCKS_PER_SEC;
    printf("Selesai dalam %.2f detik.\n", detik);

    printf("\n===== HASIL SORTING KATALOG BUKU =====\n");
    tampil_tabel_buku(salinan, jumlah_buku, hanya_tersedia);

    free(salinan);
}

void menu_sorting(void) {
    jalankan_menu_sorting(0);
}

void menu_sorting_tersedia(void) {
    jalankan_menu_sorting(1);
}
