#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  if (inb(0x64) & 0x1) {
    kbd->keycode = inl(0x60) & 0xff;
    kbd->keydown = kbd->keycode < 128;
  } else {
    kbd->keycode = AM_KEY_NONE;
    kbd->keydown = false;
  }
}
