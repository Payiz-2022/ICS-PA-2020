#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
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

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
  {"/dev/events", 0, 0, events_read, invalid_write},
  {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

#define FILES_CNT (sizeof(file_table) / sizeof(Finfo))
#define CUR_FT file_table[fd]

void init_fs() {
  AM_GPU_CONFIG_T cfg;
  ioe_read(AM_GPU_CONFIG, &cfg);
  file_table[FD_FB].size = cfg.width * cfg.height * sizeof(uint32_t);
}

int fs_open(const char *pathname, int flags, int mode){
  for (int i = 0; i < FILES_CNT; i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      #ifdef FS_DEBUG
        Log("[File System] fs_open (fd = %d): Opening file %s", i, pathname);
      #endif
      file_table[i].open_offset = 0;
      return i;
    }
  }
  panic("File %s not found", pathname);
}

size_t fs_read(int fd, void *buf, size_t len) {
  if (CUR_FT.size == 0) {
    return CUR_FT.read(buf, 0, len);
  }
  if (CUR_FT.open_offset + len > CUR_FT.size) {
    len = CUR_FT.size - CUR_FT.open_offset;
  }
  size_t ret = ramdisk_read(buf, CUR_FT.disk_offset + CUR_FT.open_offset, len);
  CUR_FT.open_offset += ret;
  #ifdef FS_DEBUG
    Log("[File System] fs_read (fd = %d): Read %d bytes, offset %d, length %d", fd, ret, CUR_FT.open_offset, len);
  #endif
  return ret;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  if (CUR_FT.size == 0) {
    return CUR_FT.write(buf, 0, len);
  }
  if (CUR_FT.open_offset + len > CUR_FT.size) {
    len = CUR_FT.size - CUR_FT.open_offset;
  }
  size_t ret = ramdisk_write(buf, CUR_FT.disk_offset + CUR_FT.open_offset, len);
  CUR_FT.open_offset += ret;
  #ifdef FS_DEBUG
    Log("[File System] fs_write (fd = %d): Write %d bytes, offset %d, length %d", fd, ret, CUR_FT.open_offset, len);
  #endif
  return ret;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  if (whence == SEEK_SET) {
    CUR_FT.open_offset = offset;
  } else if (whence == SEEK_CUR) {
    CUR_FT.open_offset += offset;
  } else if (whence == SEEK_END) {
    CUR_FT.open_offset = CUR_FT.size + offset;
  } else {
    panic("Invalid parameter for fs_lseek");
  }
  #ifdef FS_DEBUG
    Log("[File System] fs_lseek (fd = %d): moving to %d", fd, CUR_FT.open_offset);
  #endif
  return CUR_FT.open_offset;
}

int fs_close(int fd) {
  return 0;
}
