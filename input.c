#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "input.h"

#define UKURAN_BARIS 128


static void buang_sisa_baris(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}


static int baca_baris(char *buffer, int ukuran)
{
    size_t len;

    if (fgets(buffer, ukuran, stdin) == NULL) {
        buffer[0] = '\0';
        return 0;
    }
    len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';          
    } else {
        buang_sisa_baris();              
    }
    return 1;
}

int baca_int(const char *prompt, int min, int max)
{
    char baris[UKURAN_BARIS];
    char *akhir;
    long nilai;

    while (1) {
        printf("%s", prompt);
        if (!baca_baris(baris, sizeof(baris))) {
            return min;                  
        }

        errno = 0;
        nilai = strtol(baris, &akhir, 10);

        if (akhir == baris) {            
            printf("Input harus berupa angka!\n");
            continue;
        }
        while (isspace((unsigned char)*akhir)) {
            akhir++;
        }
        if (*akhir != '\0') {            
            printf("Input harus berupa angka!\n");
            continue;
        }
        if (errno == ERANGE || nilai < min || nilai > max) {
            printf("Angka harus antara %d dan %d!\n", min, max);
            continue;
        }
        return (int)nilai;
    }
}

void baca_string(const char *prompt, char *buffer, int ukuran)
{
    printf("%s", prompt);
    baca_baris(buffer, ukuran);
}

int konfirmasi(const char *prompt)
{
    char baris[UKURAN_BARIS];
    int i;

    while (1) {
        printf("%s (y/n): ", prompt);
        if (!baca_baris(baris, sizeof(baris))) {
            return 0;                   
        }
        for (i = 0; baris[i] != '\0'; i++) {
            baris[i] = (char)tolower((unsigned char)baris[i]);
        }
        if (strcmp(baris, "y") == 0 || strcmp(baris, "ya") == 0) {
            return 1;
        }
        if (strcmp(baris, "n") == 0 || strcmp(baris, "t") == 0 ||
            strcmp(baris, "tidak") == 0) {
            return 0;
        }
        printf("Jawab dengan y (ya) atau n (tidak)!\n");
    }
}
