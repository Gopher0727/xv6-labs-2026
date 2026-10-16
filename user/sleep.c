#include "kernel/types.h"
#include "user/user.h"  // 注意顺序

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(2, "Usage: sleep ticks\n");
    exit(1);
  }

  int ticks = atoi(argv[1]);
  pause(ticks);

  exit(0);
}
