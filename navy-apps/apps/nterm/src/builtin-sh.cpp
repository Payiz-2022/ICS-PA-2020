#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>
#include <stdio.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  printf("%s", buf);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void sh_handle_cmd(const char *cmd) {
  char env_name[30], env_val[80];
  if (memcmp(cmd, "exit\n", sizeof(char) * 5) == 0) {
    exit(0);
  } else if (memcmp(cmd, "echo ", sizeof(char) * 5) == 0) {
    const char* idx = cmd + 5;
    bool bslash = false;
    while (*idx) {
      if (bslash) {
        bslash = false;
        if (*idx == '\\' || *idx == '$')
          sh_printf("%c", *idx);
      } else if (*idx == '\\') {
        bslash = true;
      } else if (*idx == '$') {
        sscanf(idx, "%s", env_name);
        sh_printf("%s", getenv(env_name));
        idx--;
      } else {
        sh_printf("%c", *idx);
      }
      idx++;
    }
  } else if (sscanf(cmd, "%[^=]=%s", env_name, env_val) == 2) {
    setenv(env_name, env_val, true);
  } else {
    sscanf(cmd, "%s", env_val);
    execvp(env_val, NULL);
  }
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
