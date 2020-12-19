#include <am.h>
#include <nemu.h>
#include <klib.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  int k = inl(KBD_ADDR);
  kbd->keydown = (k & KEYDOWN_MASK ? true : false);
  if (!kbd->keydown) {
    kbd->keycode = AM_KEY_NONE;
  } else {
    kbd->keycode = k & ~KEYDOWN_MASK;
    printf("Pressed 0x%x\n", kbd->keycode);
  }
}
