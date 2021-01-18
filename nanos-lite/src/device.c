#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char* p = (char*)buf;
  for (int i = 0; i < len; i++) {
    putch(*p);
    p++;
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE) return 0;
  return snprintf(buf, len, "%s %s", ev.keydown ? "kd" : "ku", keyname[ev.keycode]);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg;
  ioe_read(AM_GPU_CONFIG, &cfg);
  return sprintf(buf, "WIDTH : %d\nHEIGHT:%d", cfg.width, cfg.height);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  len = strlen(buf);
  AM_GPU_CONFIG_T cfg;
  ioe_read(AM_GPU_CONFIG, &cfg);
  uint32_t start_x = offset / sizeof(uint32_t) % cfg.width;
  uint32_t start_y = offset / sizeof(uint32_t) / cfg.width;
  AM_GPU_FBDRAW_T ctl = {
    .x = start_x,
    .y = start_y,
    .w = len / sizeof(uint32_t),
    .h = 1,
    .pixels = (uint32_t*)buf,
    .sync = 1
  };
  ioe_read(AM_GPU_FBDRAW, &ctl);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
