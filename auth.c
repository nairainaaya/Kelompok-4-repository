#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auth.h"

#define UKURAN_BUFFER 64

static int baca_baris(const char *prompt, char *buffer, size_t ukuran)
{
    size_t panjang;
    int c;

    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buffer, (int)ukuran, stdin) == NULL) {
        return 0;
    }

    panjang = strlen(buffer);

    if (panjang > 0 && buffer[panjang - 1] == '\n') {
        buffer[--panjang] = '\0';
    } else {
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }

    if (panjang > 0 && buffer[panjang - 1] == '\r') {
        buffer[panjang - 1] = '\0';
    }

    return 1;
}

static const User *cari_user(const User users[], int jumlah_user,
                             const char *username, const char *password)
{
    int i;

    for (i = 0; i < jumlah_user; i++) {
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0) {
            return &users[i];
        }
    }
    return NULL;
}

const char *login(const User users[], int jumlah_user,
                  char *out_username, size_t ukuran_out)
{
    char input_user[UKURAN_BUFFER];
    char input_pass[UKURAN_BUFFER];
    const User *ditemukan;
    int percobaan;
    int sisa;

    if (users == NULL || out_username == NULL || ukuran_out == 0) {
        return NULL;
    }
    out_username[0] = '\0';

    printf("\n=== LOGIN SISTEM PERPUSTAKAAN ===\n");

    for (percobaan = 1; percobaan <= MAX_PERCOBAAN_LOGIN; percobaan++) {
        if (!baca_baris("Username : ", input_user, sizeof(input_user)) ||
            !baca_baris("Password : ", input_pass, sizeof(input_pass))) {
            printf("\nInput dihentikan. Login dibatalkan.\n");
            return NULL;
        }

        ditemukan = cari_user(users, jumlah_user, input_user, input_pass);

        if (ditemukan != NULL) {
            strncpy(out_username, ditemukan->username, ukuran_out - 1);
            out_username[ukuran_out - 1] = '\0';

            printf("\nLogin berhasil! Selamat datang, %s (%s).\n",
                   ditemukan->username, ditemukan->role);
            return ditemukan->role;
        }

        sisa = MAX_PERCOBAAN_LOGIN - percobaan;
        if (sisa > 0) {
            printf("\nLogin gagal: username atau password salah. "
                   "Sisa percobaan: %d\n\n", sisa);
        }
    }

    printf("\nLogin gagal %d kali berturut-turut. Akses ditolak.\n",
           MAX_PERCOBAAN_LOGIN);
    return NULL;
}