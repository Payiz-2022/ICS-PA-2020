#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
# define Elf_Off Elf64_Off
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define Elf_Off Elf32_Off
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);

  Elf_Ehdr buf_Eheader;
  fs_read(fd, (void*)&buf_Eheader, sizeof(buf_Eheader));

  fs_lseek(fd, buf_Eheader.e_phoff, SEEK_SET);

  Elf_Phdr buf_Pheader[buf_Eheader.e_phnum];
  for (int i = 0; i < buf_Eheader.e_phnum; i++) {
    // Read from each program header
    fs_read(fd, (void*)&buf_Pheader[i], buf_Eheader.e_phentsize);
  }

  for (int i = 0; i < buf_Eheader.e_phnum; i++) {
    fs_lseek(fd, buf_Pheader[i].p_offset, SEEK_SET);
    fs_read(fd, (void*)buf_Pheader[i].p_vaddr, buf_Pheader[i].p_filesz);
    memset((void*)(buf_Pheader[i].p_vaddr + buf_Pheader[i].p_filesz), 0, buf_Pheader[i].p_memsz - buf_Pheader[i].p_filesz);
  }

  return buf_Eheader.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}
