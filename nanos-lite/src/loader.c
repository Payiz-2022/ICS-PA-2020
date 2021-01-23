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
      if (buf_Pheader[i].p_memsz == 0) continue;
      uintptr_t mem_top = buf_Pheader[i].p_vaddr;
      for (int j = buf_Pheader[i].p_vaddr >> 12; j <= (buf_Pheader[i].p_vaddr + buf_Pheader[i].p_memsz - 1) >> 12; j++) {
        uintptr_t next_mem_top = min(buf_Pheader[i].p_vaddr, (mem_top & ADDRMASK) + PGSIZE);
        pcb->max_brk = next_mem_top;

        void* paddr = pg_alloc(PGSIZE);
        map(&pcb->as, (void*)mem_top, paddr, 0);
        fs_read(fd, paddr + (mem_top & FLAGMASK), 
            max(0, min(next_mem_top - mem_top, buf_Pheader[i].p_vaddr + buf_Pheader[i].p_filesz - mem_top)));

        #ifdef DEBUG
          Log("[Loader] Loading new page 0x%08x (from 0x%08x)", paddr, mem_top);
        #endif

        mem_top = next_mem_top;
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
#ifdef HAS_VME
  protect(&pcb->as);
#endif
  uintptr_t entry = loader(pcb, filename);
  if (entry == 0) {pcb->cp = NULL; return;}

  Area stack;
  stack.start = (void*)pcb;
  stack.end = stack.start + sizeof(PCB);

#ifdef HAS_VME
  pcb->cp = ucontext(&pcb->as, stack, (void*)entry);
#else
  pcb->cp = ucontext(NULL, stack, (void*)entry);
#endif

  void* mem_top = new_page(8);
#ifdef HAS_VME
  map(&pcb->as, pcb->as.area.end - 8 * PGSIZE, mem_top, 0);
#endif

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
