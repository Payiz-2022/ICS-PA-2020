void init_monitor(int, char *[]);
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  init_monitor(argc, argv);

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}


// char result[1000], exp[62000];
// FILE *fin = fopen("input", "r");
// assert(fin != NULL);
// while (fscanf(fin, "%s %s", result, exp) == 2) {
//   word_t ans = 0;
//   sscanf(result, "%u", &ans);
//   bool success = 0;
//   word_t res = expr(exp, &success);
//   if (!success) puts("Parse failed");
//   else if (res != ans) puts("Not matched");
//   else puts("Success");
// }