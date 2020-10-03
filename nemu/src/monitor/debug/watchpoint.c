#include "watchpoint.h"
#include "expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
// static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].exp = NULL;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp() {
  assert(free_ != NULL);
  if (head == NULL) {
    head = free_;
  } else {
    WP *p = head;
    while (p->next != NULL) p = p->next;
    p->next = free_;
  }
  WP* ans = free_;
  free_ = free_->next;
  return ans;
}

void free_wp(WP *wp) {
  assert(wp != NULL);
  if (free_ == NULL) {
    free_ = wp;
  } else {
    WP *p = free_;
    while (p->next != NULL) p = p->next;
    p->next = wp;
  }
  wp->next = NULL;
  if (head == wp) {
    head = wp->next;
  } else {
    WP *p = head;
    while (p->next != wp) {
      assert(p != NULL);
      p = p->next;
    }
    p->next = p->next->next;
  }
}

void print_all_wps() {
  WP *p = head;
  while (p) {
    printf("Breakpoint %d: %s (value = %u)\n", p->NO, p->exp, p->exp_val);
    p = p->next;
  }
}