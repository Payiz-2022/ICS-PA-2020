#include <proc.h>
#include <elf.h>

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
  Elf_Ehdr buf_Eheader;
  ramdisk_read((void*)&buf_Eheader, 0, sizeof(buf_Eheader));

  Elf_Off p_off = buf_Eheader.e_phoff;
  for (int i = 0; i < buf_Eheader.e_phnum; i++) {
    // Read from each program header
    Elf_Phdr buf_Pheader;
    ramdisk_read((void*)&buf_Pheader, p_off, buf_Eheader.e_phentsize);
    p_off += buf_Eheader.e_phentsize;

    ramdisk_read((void*)buf_Pheader.p_vaddr, buf_Pheader.p_offset, buf_Pheader.p_filesz);
    memset((void*)(buf_Pheader.p_vaddr + buf_Pheader.p_filesz), 0, buf_Pheader.p_memsz - buf_Pheader.p_filesz);
  }

  return buf_Eheader.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}
