#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "user/user.h"

// struct dirent {
//   ushort inum;
//   char   namep[DIRSIZ];
// };

static void run(char* path, char** cmd, int argc) {
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "find: fork failed\n");
    return;
  }

  if (pid == 0) {
    char* args[MAXARG];
    for (int i = 0; i < argc; i++) {
      args[i] = cmd[i];
    }
    args[argc]     = path;
    args[argc + 1] = 0;

    exec(args[0], args);

    fprintf(2, "find: exec %s failed\n", args[0]);
    exit(1);
  }

  wait(0);
}

static void find(char* path, char* target, char** cmd, int argc) {
  struct stat st;
  if (stat(path, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    return;
  }

  // 获取文件名
  char* name = path + strlen(path);
  while (name > path && name[-1] != '/') {
    name--;
  }

  if (strcmp(name, target) == 0) {
    if (cmd == 0) {
      printf("%s\n", path);
    } else {
      run(path, cmd, argc);
    }
  }

  if (st.type != T_DIR) {
    return;
  }

  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  struct dirent de;
  char          buf[512];

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0) {
      continue;
    }

    char entry[DIRSIZ + 1];
    memmove(entry, de.name, DIRSIZ);
    entry[DIRSIZ] = '\0';

    // 防止无限递归
    if (strcmp(entry, ".") == 0 || strcmp(entry, "..") == 0) {
      continue;
    }

    int plen = strlen(path), elen = strlen(entry);
    if (plen + 1 + elen + 1 > sizeof(buf)) {
      fprintf(2, "find: path too long\n");
      continue;
    }

    strcpy(buf, path);
    buf[plen] = '/';
    strcpy(buf + plen + 1, entry);

    find(buf, target, cmd, argc);
  }

  close(fd);
}

int main(int argc, char* argv[]) {
  if (argc != 3 && (argc < 5 || strcmp(argv[3], "-exec") != 0)) {
    fprintf(2, "Usage: find path name [-exec cmd args...]\n");
    exit(1);
  }

  if (argc == 3) {
    find(argv[1], argv[2], 0, 0);
  } else {
    int cmdargc = argc - 4;
    if (cmdargc + 2 > MAXARG) {
      fprintf(2, "find: too many arguments\n");
      exit(1);
    }
    find(argv[1], argv[2], &argv[4], cmdargc);
  }

  exit(0);
}
