#include <am.h>
#include <nemu.h>
#include <klib.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t k = inl(KBD_ADDR);
  kbd->keydown = (k & KEYDOWN_MASK ? true : false);
  printf("keycode: %x keydown: %x\n", k, kbd->keydown);
  if (!kbd->keydown) {
    kbd->keycode = AM_KEY_NONE;
  } else {
    kbd->keycode = k & ~KEYDOWN_MASK;
  }
}
