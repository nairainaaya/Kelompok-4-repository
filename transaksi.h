#ifndef TRANSAKSI_H
#define TRANSAKSI_H

#include "data.h"

#define STATUS_DIPINJAM "DIPINJAM"
#define STATUS_KEMBALI  "KEMBALI"

void pinjam_buku(const char *username);

void kembalikan_buku(const char *username);

void tampil_pinjaman_saya(const char *username);

void tampil_laporan_transaksi(void);

unsigned int buat_id_transaksi(void);

void ambil_tanggal_hari_ini(char *out);

#endif 
