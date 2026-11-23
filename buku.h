#ifndef BUKU_H
#define BUKU_H

#include "data.h"

void tambah_buku(void);
void ubah_buku(void);
void hapus_buku(void);

void tampil_katalog(int hanya_tersedia);

void tampil_tabel_buku(const Buku arr[], int n, int hanya_tersedia);

void cari_buku_by_id(void);
void cari_buku_by_judul(void);

int cari_index_buku(const char *ocaid);

#endif
