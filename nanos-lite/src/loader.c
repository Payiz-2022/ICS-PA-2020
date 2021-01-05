#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#define PMEM_START 0x3000000

static uintptr_t loader(PCB *pcb, const char *filename) {
  ramdisk_read((void*)PMEM_START, 0, get_ramdisk_size());
  return *(int*)(PMEM_START + 0x18);
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

