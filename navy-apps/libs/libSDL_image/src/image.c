#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"
#include <stdio.h>

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  FILE *img = fopen(filename, "r");
  assert(img);
  fseek(img, 0, SEEK_END);
  size_t size = ftell(img);
  fseek(img, 0, SEEK_SET);

  unsigned char* buf = malloc(size);
  fread(buf, size, 1, img);
  SDL_Surface* ret = STBIMG_LoadFromMemory(buf, size);
  fclose(img);
  free(buf);

  return ret;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
