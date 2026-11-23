#ifndef INPUT_H
#define INPUT_H

/* Baca bilangan bulat dalam rentang [min, max]; ulang sampai valid */
int baca_int(const char *prompt, int min, int max);

/* Baca satu baris teks ke buffer (maks ukuran-1 karakter), newline dibuang */
void baca_string(const char *prompt, char *buffer, int ukuran);

/* Tanya ya/tidak; kembalikan 1 jika ya, 0 jika tidak */
int konfirmasi(const char *prompt);

#endif
