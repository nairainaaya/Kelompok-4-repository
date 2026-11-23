#ifndef SORTING_H
#define SORTING_H

#include "data.h"

void bubble_sort(Buku arr[], int n,
                 int (*bandingkan)(const Buku *, const Buku *));
void insertion_sort(Buku arr[], int n,
                    int (*bandingkan)(const Buku *, const Buku *));
void selection_sort(Buku arr[], int n,
                    int (*bandingkan)(const Buku *, const Buku *));

int banding_id_asc(const Buku *a, const Buku *b);
int banding_id_desc(const Buku *a, const Buku *b);
int banding_judul_asc(const Buku *a, const Buku *b);
int banding_judul_desc(const Buku *a, const Buku *b);
int banding_tahun_asc(const Buku *a, const Buku *b);
int banding_tahun_desc(const Buku *a, const Buku *b);
int banding_halaman(const Buku *a, const Buku *b);       
int banding_halaman_desc(const Buku *a, const Buku *b);
int banding_stok(const Buku *a, const Buku *b);          
int banding_stok_desc(const Buku *a, const Buku *b);

void menu_sorting(void);
void menu_sorting_tersedia(void);

#endif 
