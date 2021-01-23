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
  if (fd == 0) return 0;

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
    #ifndef HAS_VME
      fs_read(fd, (void*)buf_Pheader[i].p_vaddr, buf_Pheader[i].p_filesz);
      memset((void*)(buf_Pheader[i].p_vaddr + buf_Pheader[i].p_filesz), 0, buf_Pheader[i].p_memsz - buf_Pheader[i].p_filesz);
    #else
      uint32_t loaded_mem = 0;
      while (loaded_mem < buf_Pheader[i].p_memsz) {
        int32_t load_size = min(buf_Pheader[i].p_filesz - loaded_mem, PGSIZE);
        if (load_size < 0) load_size = 0;

        void* paddr = pg_alloc(PGSIZE);
        fs_read(fd, paddr, load_size);
        map(&pcb->as, (void*)buf_Pheader[i].p_vaddr + loaded_mem, paddr, 0);

        loaded_mem += load_size;
      }
    #endif
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
  // Log("Loading user process [%s] at pcb 0x%08x\n", filename, pcb);
  protect(&pcb->as);
  uintptr_t entry = loader(pcb, filename);
  if (entry == 0) {pcb->cp = NULL; return;}

  Area stack;
  stack.start = (void*)pcb;
  stack.end = stack.start + sizeof(PCB);

  pcb->cp = ucontext(&pcb->as, stack, (void*)entry);

  void* mem_top = new_page(8);
  map(&pcb->as, pcb->as.area.end - 8 * PGSIZE, mem_top, 0);

  int argc = 0;
  void* argv_start = mem_top;
  char*const* p = argv;
  while (p && *p) {
    argv_start -= strlen(*p) + 1;
    argc++; p++;
  }
  argv_start -= 8;
  argv_start -= argc * sizeof(char*);

  p = argv;
  while (p && *p) {
    mem_top -= strlen(*p) + 1;
    strcpy(mem_top, *p);
    *(uintptr_t*)argv_start = (uintptr_t)mem_top;
    argv_start += sizeof(uintptr_t);
    p++;
  }
  mem_top -= 4;
  *(uintptr_t*)mem_top = 0;
  mem_top -= 4;
  *(uintptr_t*)mem_top = 0;
  mem_top -= argc * sizeof(char*);
  mem_top -= 4;
  *(intptr_t*)mem_top = argc;

  pcb->cp->GPRx = (uintptr_t)mem_top;
}
