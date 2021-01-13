#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define FILES_CNT (sizeof(file_table) / sizeof(Finfo))
#define CUR_FT file_table[fd]

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode){
  for (int i = 0; i < FILES_CNT; i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      Log("Opening file %s (fd = %d)", pathname, i);
      return i;
    }
  }
  panic("File %s not found", pathname);
}

size_t fs_read(int fd, void *buf, size_t len) {
  Log("fs_read  offset: %d, len: %d, size: %d", CUR_FT.disk_offset, len, CUR_FT.size);
  assert(CUR_FT.disk_offset + len <= CUR_FT.size);
  size_t ret = ramdisk_read(buf, CUR_FT.disk_offset, len);
  CUR_FT.disk_offset += len;
  return ret;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  assert(CUR_FT.disk_offset + len <= CUR_FT.size);
  size_t ret = ramdisk_write(buf, CUR_FT.disk_offset, len);
  CUR_FT.disk_offset += len;
  return ret;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  if (whence == SEEK_SET) {
    CUR_FT.disk_offset = offset;
  } else if (whence == SEEK_CUR) {
    CUR_FT.disk_offset += offset;
  } else if (whence == SEEK_END) {
    CUR_FT.disk_offset = CUR_FT.size + offset;
  } else {
    panic("Invalid parameter for fs_lseek");
  }
  return CUR_FT.disk_offset;
}

int fs_close(int fd) {
  return 0;
}
