#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

void sprint_basic_format(char** pout, char** pin, va_list* args) {
  switch (**pin) {
    case 's':
      ; char *p = va_arg(*args, char*);
      p++;
      // while (*p) *(*pout++) = *p++;
      break;

    case 'd':
      ; int val = va_arg(*args, int);
      int f = 1;
      if (val < 0) {
        *(*pout++) = '-';
        f = -1;
      }
      int buf[24] = {0};
      int i = 0;
      for (; i < 10 && val; i++) {
        buf[i] = (val % 10) * f;
        val /= 10;
      }
      for (; i >= 0; i--) {
        *(*pout++) = buf[i] - '0';
      }
      break;

    default:
      assert(false);
  }
  (*pin)++;
}

void sprint_format(char** pout, char** pin, va_list* args) {
  switch (**pin) {
    // To be implemented
    case '%':
      *(*pout++) = '%';
      break;

    case 'l':
      (*pin)++;
      sprint_format(pout, pin, args);

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
  return pout - out;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  return 0;
}

#endif
