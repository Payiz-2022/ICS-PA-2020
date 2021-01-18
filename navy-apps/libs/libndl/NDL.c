#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0, canvas_w = 0, canvas_h = 0;
static uint32_t* canvas;

uint32_t NDL_GetTicks() {
  struct timeval current_time;
  gettimeofday(&current_time, NULL);
  return current_time.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  FILE* evt_file = fopen("/dev/events", "r");
  char ch = -1;
  int i = 0;
  while (i < len && (ch = fgetc(evt_file)) != EOF) {
    buf[i++] = ch;
  }
  return i ? 1 : 0;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  } else {
    canvas_w = *w; canvas_h = *h;
    assert(canvas_w <= screen_w && canvas_h <= screen_h);
    canvas = malloc(canvas_w * canvas_h * sizeof(uint32_t));
  }
}

void NDL_UpdateCanvas() {
  FILE* fb_file = fopen("/dev/fb", "w");
  printf("Info: screen %d*%d, canvas %d*%d\n", screen_w, screen_h, canvas_w, canvas_h);
  for (int i = 0; i < screen_h; i++)
    for (int j = 0; j < screen_w; j++) {
      if (i < canvas_h && j < canvas_w) {
        fwrite(&canvas[i * canvas_w + j], sizeof(uint32_t), 1, fb_file);
      } else {
        fprintf(fb_file, "0000");
      }
    }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  for (int i = 0; i < h; i++)
    for (int j = 0; j < w; j++)
      canvas[(y + i) * canvas_w + (x + j)] = pixels[i * w + j];
  NDL_UpdateCanvas();
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  FILE* dispinfo_file = fopen("/proc/dispinfo", "r");
  fscanf(dispinfo_file, "WIDTH : %d\nHEIGHT:%d", &screen_w, &screen_h);
  return 0;
}

void NDL_Quit() {
}
