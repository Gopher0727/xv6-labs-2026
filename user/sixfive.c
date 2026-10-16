#include "kernel/types.h"
#include "user/user.h"

const char* seps = " -\r\t\n./,";

void display(char* token) {
  char* p = token;
  while (*p == '0' && p[1] != '0') {
    p++;
  }
  printf("%s\n", p);
}

static int process(int fd) {
  int cap = 64;
  int len = 0;
  int ok  = 1;
  int rem = 0;

  char* token = malloc(cap);
  if (token == 0) {
    return -1;
  }

  char c;
  int  n;
  while ((n = read(fd, &c, 1)) == 1) {
    if (strchr(seps, c)) {
      if (ok && len > 0 && (rem % 5 == 0 || rem % 6 == 0)) {
        token[len] = '\0';
        display(token);
      }
      len = 0;
      ok  = 1;
      rem = 0;
      continue;
    }
    if (c < '0' || c > '9') {
      ok = 0;
      continue;
    }
    if (!ok) {
      continue;
    }

    if (len + 1 >= cap) {
      int   ncap = cap << 1;
      char* tmp  = malloc(ncap);
      if (tmp == 0) {
        free(token);
        return -1;
      }

      memmove(tmp, token, len);
      free(token);

      token = tmp;
      cap   = ncap;
    }

    token[len++] = c;
    // 计算模 30
    rem = (rem * 10 + c - '0') % 30;
  }

  if (n == 0 && ok && len > 0 && (rem % 5 == 0 || rem % 6 == 0)) {
    token[len] = '\0';
    display(token);
  }

  free(token);
  return n < 0 ? -1 : 0;
}

int main(int argc, char* argv[]) {
  int status = 0;
  if (argc == 1) {
    if (process(0) < 0) {
      return -1;
    }
  } else {
    for (int i = 1; i < argc; i++) {
      int fd = open(argv[i], 0);
      if (fd < 0) {
        fprintf(2, "sixfive: cannot open %s\n", argv[i]);
        status = 1;
        continue;
      }
      if (process(fd) < 0) {
        status = 1;
      }
      close(fd);
    }
  }
  exit(status);
}
