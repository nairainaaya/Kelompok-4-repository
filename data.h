#ifndef DATA_H
#define DATA_H

typedef struct {
    char ocaid[64];
    char title[1280];
    char author[512];
    int  year;
    int  page_count;
    int  stok;
} Buku;

typedef struct {
    char username[32];
    char password[32];
    char role[12];         
} User;

typedef struct {
    unsigned int id_peminjaman;
    char username[32];
    char ocaid[64];
    char tanggal_pinjam[20];  
    char status[20];          
} Peminjaman;

extern Buku       *daftar_buku;
extern int         jumlah_buku;
extern User       *daftar_user;
extern int         jumlah_user;
extern Peminjaman *daftar_pinjam;
extern int         jumlah_pinjam;

void muat_buku_dari_asli(void);   
void simpan_katalog(void);        
void muat_katalog(void);        

void muat_user(void);
void simpan_user(void);

void muat_peminjaman(void);
void simpan_peminjaman(void);

int tambah_buku_ke_array(const Buku *b);
int tambah_peminjaman_ke_array(const Peminjaman *p);

void bebaskan_semua(void);

#endif 
