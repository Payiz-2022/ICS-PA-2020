#include <isa.h>
#include "expr.h"
#include "watchpoint.h"
#include <memory/paddr.h>
#include <monitor/monitor.h>

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  int num = 1;
  if (args != NULL)
    sscanf(args, "%d", &num);
  while (num--) {
    cpu_exec(-1);
    if (nemu_state.state == NEMU_QUIT || 
        nemu_state.state == NEMU_ABORT ||
        nemu_state.state == NEMU_END) return 0;
  }
  return 0;
}

// char result[1000], exp[62000];
// FILE *fin = fopen("input", "r");
// assert(fin != NULL);
// int count = 0;
// while (fscanf(fin, "%s %s", result, exp) == 2) {
//   count++;
//   word_t ans = 0;
//   sscanf(result, "%u", &ans);
//   bool success = 0;
//   word_t res = expr(exp, &success);
//   if (success && res == ans) continue;
//   printf("Expression: %s, answer = %s, res = %u\n", exp, result, res);
//   if (!success) {
//     puts("Failed to parse");
//   } else if (res != ans) {
//     puts("Wrong answer");
//   }
// }


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
  int num = 1;
  if (args != NULL)
    sscanf(args, "%d", &num);
  cpu_exec(num);
  return 0;
}

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_w(char *args);

static int cmd_p(char *args);

static int cmd_d(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  {"help", "Display informations about all supported commands.", cmd_help},
  {"c", "Continue the execution of the program.", cmd_c},
  {"q", "Exit NEMU.", cmd_q},
  {"si", "Step one instruction exactly, argument N means step N times (or till program stops for another reason).", cmd_si},
  {"info", "Generic command for showing things about the program being debugged.", cmd_info},
  {"x", "Read from the memory of the current target process.", cmd_x},
  {"w", "Set watchpoint for expression EXP.", cmd_w},
  {"p", "Print value of expression EXP.", cmd_p},
  {"d", "Remove watchpoint by ID.", cmd_d}
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_info(char *args) {
  char *subcmd = strtok(NULL, " ");

  /* Show help information when subcommand is illegal */
  if (subcmd == NULL || strlen(subcmd) != 1) {
    for (int i = 0; i < NR_CMD; i ++) {
      if (strcmp("info", cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    return 0;
  }

  /* Deal with subcommand */
  switch(subcmd[0]) {
    case 'r':
      isa_reg_display();
      break;
    case 'w':
      print_all_wps();
      break;
    case 'f':
      printf("General Flags:\n");
      printf("  eflags: 0x%x\n", cpu.eflags.val);
      printf("      CF: %u\n", cpu.eflags.CF & 1);
      printf("      ZF: %u\n", cpu.eflags.ZF & 1);
      printf("      SF: %u\n", cpu.eflags.SF & 1);
      printf("      IF: %u\n", cpu.eflags.IF & 1);
      printf("      OF: %u\n", cpu.eflags.OF & 1);
      break;
  }
  return 0;
}

static int cmd_x(char *args) {
  char *n = strtok(NULL, " ");
  char *exp = strtok(NULL, " ");
  int num;
  word_t addr;
  sscanf(n, "%d", &num);

  bool success = false;
  addr = expr(exp, &success);
  if (!success) {
    printf("Invalid Expression\n");
    return 0;
  }
  
  printf("0x%08x:", addr);
  int i = 0;
  while (num--) {
    printf(" %02x", paddr_read(addr + (i++), 1));
  }
  putchar('\n');
  return 0;
}

static int cmd_w(char *args) {
  char *exp = strtok(NULL, " ");
  bool success = false;
  word_t val = expr(exp, &success);
  if (!success){ 
    printf("Invalid Expression\n");
  } else {
    WP* p = new_wp();
    memcpy(p->exp, exp, sizeof(char) * strlen(exp));
    p->exp_val = val;
    printf("Watchpoint %d set at %s (value = %u)\n", p->NO, p->exp, p->exp_val);
  }
  return 0;
}

static int cmd_p(char *args) {
  char *exp = args;
  bool success = false;
  word_t val = expr(exp, &success);
  if (!success) {
    printf("Invalid Expression\n");
  } else {
    printf("%u (0x%x)\n", val, val);
  }
  return 0;
}

static int cmd_d(char *args) {
  char *no = strtok(NULL, " ");
  int wp_no = 0;
  sscanf(no, "%d", &wp_no);
  if (remove_wp(wp_no)) {
    printf("Watchpoint %d removed.\n", wp_no);
  } else {
    printf("Error: Cannot find watchpoint %d.\n", wp_no);
  }
  return 0;
}

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
