#include <isa.h>
#include "expr.h"
#include "watchpoint.h"

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
  char result[1000], exp[62000];
  freopen("input", "r", stdin);
  freopen("output", "w", stdout);
  while (scanf("%s %s", result, exp) == 2) {
    word_t ans = 0;
    sscanf(result, "%u", &ans);
    bool success = 0;
    word_t res = expr(exp, &success);
    if (!success) puts("Parse failed");
    else if (res != ans) puts("Not matched");
    else puts("Success");
  }
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
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
      break;
  }
  return 0;
}

static int cmd_x(char *args) {
  char *n = strtok(NULL, " ");
  char *exp = strtok(NULL, " ");
  int num;
  char *addr;
  sscanf(n, "%d", &num);
  sscanf(exp, "0x%p", &addr);

  printf("%s:", exp);
  int i = 0;
  while (num--) {
    printf(" %02x", (char)addr[i++] & 0xff);
  }
  putchar('\n');
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
