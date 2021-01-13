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

  for (int i = 0; i < buf_Eheader.e_phnum; i++) {
    // Read from each program header
    Elf_Phdr buf_Pheader;
    fs_read(fd, (void*)&buf_Pheader, buf_Eheader.e_phentsize);

    Log("[Loader] Load program header (vaddr = 0x%x, filesz = 0x%x)", buf_Pheader.p_vaddr, buf_Pheader.p_filesz);
    fs_read(fd, (void*)buf_Pheader.p_vaddr, buf_Pheader.p_filesz);
    memset((void*)(buf_Pheader.p_vaddr + buf_Pheader.p_filesz), 0, buf_Pheader.p_memsz - buf_Pheader.p_filesz);
  }

  return buf_Eheader.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}
