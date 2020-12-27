#include <am.h>
#include <nemu.h>
#include <klib.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t k = inl(KBD_ADDR);
  if (k == AM_KEY_NONE) return;
  kbd->keydown = (k & KEYDOWN_MASK ? true : false);
  if (!kbd->keydown) {
    kbd->keycode = AM_KEY_NONE;
  } else {
    kbd->keycode = k & ~KEYDOWN_MASK;
  }
}
