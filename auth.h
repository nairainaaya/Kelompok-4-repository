#ifndef AUTH_H
#define AUTH_H

#include <stddef.h>

#define MAX_PERCOBAAN_LOGIN 3

#include "data.h"

const char *login(const User users[], int jumlah_user,
                  char *out_username, size_t ukuran_out);

#endif 