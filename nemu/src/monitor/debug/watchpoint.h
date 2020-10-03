#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char* exp;
  word_t exp_val;
} WP;

WP *head, *free_;

void init_wp_pool();
WP* new_wp();
void free_wp(WP *wp);
void print_all_wps();

#endif
