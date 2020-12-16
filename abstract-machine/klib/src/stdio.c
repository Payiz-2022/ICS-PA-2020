#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char s[100];
  sprintf(s, fmt, args);
  char *p = s;
  while (*p) {
    putch(*p);
    p++;
  }
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

static struct {
  int lpad;
} pref;

void sprint_basic_format(char** pout, char** pin, va_list* args) {
  switch (**pin) {
    case 's':
      ; char *p = va_arg(*args, char*);
      while (*p) *(*pout)++ = *p++;
      break;

    case 'd':
      ; int val = va_arg(*args, int);
      int f = 1;
      if (val < 0) {
        *(*pout)++ = '-';
        f = -1;
      }
      int buf[24] = {0};
      int i = 0;
      for (; i < 10 && val; i++) {
        buf[i] = (val % 10) * f;
        val /= 10;
      }
      // if (i < pref.lpad) i = pref.lpad;
      for (i--; i >= 0; i--) {
        *(*pout)++ = buf[i] + '0';
      }
      break;

    default:
      assert(false);
  }
  (*pin)++;
}

int sprint_read_pad(char** pout, char** pin, va_list* args) {
  int sum = **pin - '0';
  if (sum < '0' || sum > '9') return 0;
  (*pin)++;
  // int ans = sprint_read_pad(pout, pin, args);
  // if (ans) return sum * 10 + ans;
  return sum;
}

void sprint_format(char** pout, char** pin, va_list* args) {
  switch (**pin) {
    // To be implemented
    case '%':
      *(*pout)++ = '%';
      break;

    case '0':
      (*pin)++;
      pref.lpad = sprint_read_pad(pout, pin, args);
      sprint_format(pout, pin, args);
      break;

    case 'l':
      (*pin)++;
      sprint_format(pout, pin, args);
      break;

    default:
      sprint_basic_format(pout, pin, args);
  }
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);


  char *pout = out;
  char *pin = (void*)fmt;
  while (*pin) {
    switch (*pin) {
      case '%':
        pin++;
        sprint_format(&pout, &pin, &args);
      default:
        *pout = *pin;
        pin++;
        pout++;
    }
  }
  va_end(args);
  *pout = 0;
  return pout - out;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  return 0;
}

#endif
