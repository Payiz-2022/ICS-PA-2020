#include <am.h>
#include <nemu.h>

uint32_t boot_time;

void __am_timer_init() {
  boot_time = inl(RTC_ADDR);
  putch('\n');
  putch('B');
  putch('O');
  putch('O');
  putch('T');
  putch('T');
  putch('I');
  putch('M');
  putch('E');
  putch(' ');
  unsigned int t = boot_time;
  int buf[20] = {0}, i = 0;
  while (t) {
    buf[++i] = t % 10;
    t /= 10;
  }
  if (i == 0) i++;
  while (i--) {
    putch('0' + buf[i]);
  }
  putch('\n');
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = inl(RTC_ADDR) - boot_time;
  putch('\n');
  putch('U');
  putch('P');
  putch('T');
  putch('I');
  putch('M');
  putch('E');
  putch(' ');
  unsigned int t = uptime->us;
  int buf[20] = {0}, i = 0;
  while (t) {
    buf[++i] = t % 10;
    t /= 10;
  }
  if (i == 0) i++;
  while (i--) {
    putch('0' + buf[i]);
  }
  putch('\n');
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
