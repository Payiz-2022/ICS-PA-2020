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
    #ifdef DEBUG
      Log("[Loader] Load program header (vaddr = 0x%x, filesz = 0x%x)", buf_Pheader[i].p_vaddr, buf_Pheader[i].p_filesz);
    #endif
  }

  for (int i = 0; i < buf_Eheader.e_phnum; i++) {
    fs_lseek(fd, buf_Pheader[i].p_offset, SEEK_SET);
    fs_read(fd, (void*)buf_Pheader[i].p_vaddr, buf_Pheader[i].p_filesz);
    memset((void*)(buf_Pheader[i].p_vaddr + buf_Pheader[i].p_filesz), 0, buf_Pheader[i].p_memsz - buf_Pheader[i].p_filesz);
  }
  
  fs_lseek(fd, 0, SEEK_SET);

  return buf_Eheader.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, const void* entry, void* arg) {
  Area stack;
  stack.start = (void*)pcb;
  stack.end = stack.start + sizeof(PCB);

  pcb->cp = kcontext(stack, entry, arg);
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
  printf("Loading user process [%s] at pcb 0x%08x\n", filename, pcb);
  uintptr_t entry = loader(pcb, filename);

  Area stack;
  stack.start = (void*)pcb;
  stack.end = stack.start + sizeof(PCB);

  pcb->cp = ucontext(NULL, stack, (void*)entry);

  int argc = 0;
  void* mem_top = heap.end;
  void* argv_start = heap.end;
  char*const* p = argv;
  while (p && *p) {
    argv_start -= strlen(*p) + 1;
    argc++; p++;
  }
  argv_start -= 8;
  argv_start -= argc * sizeof(char*);
  printf("calc argv start: 0x%08x\n", argv_start);

  p = argv;
  while (p && *p) {
    mem_top -= strlen(*p) + 1;
    strcpy(mem_top, *p);
    *(uintptr_t*)argv_start = (uintptr_t)mem_top;
    printf("+args: %s\n", *(uintptr_t*)argv_start);
    argv_start += sizeof(uintptr_t);
    p++;
  }
  mem_top -= 4;
  *(uintptr_t*)mem_top = 0;
  mem_top -= 4;
  *(uintptr_t*)mem_top = 0;
  mem_top -= argc * sizeof(char*);
  printf("ref memtop: 0x%08x\n", mem_top);
  // memcpy(mem_top, argv, sizeof(argc * sizeof(char*)));
  mem_top -= 4;
  *(intptr_t*)mem_top = argc;

  pcb->cp->GPRx = (uintptr_t)mem_top;
}
