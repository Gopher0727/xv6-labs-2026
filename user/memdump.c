#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "user/user.h"

void memdump(char* fmt, char* data, int len);

int main(int argc, char* argv[]) {
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char*)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char* s = "another";
    memdump("s", (char*)&s, sizeof(s));

    struct sss {
      char* ptr;
      int   num1;
      short num2;
      char  byte;
      char  bytes[8];
    } example;

    example.ptr  = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char*)&example, sizeof(example));

    printf("Example 5:\n");
    memdump("sccccc", (char*)&example, sizeof(example));
  } else if (argc == 2) {
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int  n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0) break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void memdump(char* fmt, char* data, int len) {
  // Your code here.  `data` holds `len` valid bytes.
  int offset = 0;

  for (; *fmt; fmt++) {
    int size = 0;

    switch (*fmt) {
      case 'i':
        size = 4;
        break;
      case 'p':
      case 's':
        size = 8;
        break;
      case 'h':
        size = 2;
        break;
      case 'c':
        size = 1;
        break;
      case 'S':
        while (offset < len && data[offset] != '\0') {
          printf("%c", data[offset++]);
        }
        printf("\n");
      default:
        continue;
    }

    if (offset > len || size > len - offset) {
      printf("memdump: not enough data for '%c'\n", *fmt);
      return;
    }

    switch (*fmt) {
      case 'i': {
        int value;
        memmove(&value, data + offset, 4);
        printf("%d\n", value);
      } break;
      case 'p': {
        uint64 value;
        memmove(&value, data + offset, 8);
        printf("%lx\n", value);
      } break;
      case 's': {
        char* str;
        memmove(&str, data + offset, 8);
        printf("%s\n", str);
      } break;
      case 'h': {
        short value;
        memmove(&value, data + offset, 2);
        printf("%d\n", value);
      } break;
      case 'c': {
        printf("%c\n", data[offset]);
      } break;
    }

    offset += size;
  }
}
