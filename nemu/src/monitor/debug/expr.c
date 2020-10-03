#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_DEC, TK_HEX, TK_AND, TK_NEQ, TK_REG, TK_DEREF
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {
  {" +", TK_NOTYPE},    // spaces
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       // not equal
  {"&&", TK_AND},       // and
  {"\\+", '+'},         // plus
  {"-", '-'},           // minus
  {"\\*", '*'},         // multiply
  {"/", '/'},           // devide
  {"\\(", '('},         // lbracket
  {"\\)", ')'},         // rbracket
  {"[0-9]+", TK_DEC},   // decimal
  {"0x[0-9]+", TK_HEX}, // hexadecimal
  {"\\$[A-Za-z0-9]+", TK_REG},      // register
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);


        // Return error when the number of units or tokens exceeded the length limit
        if (substr_len >= 32) return false;
        if (nr_token >= 32) return false;

        // Insert unit into tokens
        tokens[nr_token].type = rules[i].token_type;
        memcpy(tokens[nr_token].str, e + position, sizeof(char) * substr_len);
        tokens[nr_token].str[substr_len] = '\0';
        nr_token++;

        position += substr_len;

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            nr_token--;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

int checkParentheses(int start, int end) {
  int par_level = 0;
  assert(start <= end);

  for (int i = start; i <= end; i++) {
    if (tokens[i].type == '(') {
      par_level++;
    } else if (tokens[i].type == ')') {
      par_level--;
      assert(par_level >= 0);
      if (par_level == 0 && i != end) {
        return false;
      }
    }
  }
  assert(par_level == 0);
  if (tokens[start].type != '(' || tokens[end].type != ')') return false;
  return true;
}

int findMainOp(int start, int end) {
  int par_level = 0, priority = 100, main_operator = -1;
  for (int i = start; i <= end; i++) {
    if (tokens[i].type == '(') {
      par_level++;
    } else if (tokens[i].type == ')') {
      par_level--;
    }
    if (par_level) continue;
    
    if ((tokens[i].type == '*' || tokens[i].type == '/') && priority == 100) {
      main_operator = i;
    } else if ((tokens[i].type == '+' || tokens[i].type == '-') && priority >= 50) {
      main_operator = i;
      priority = 50;
    } else if (tokens[i].type == TK_AND && priority >= 30) {
      main_operator = i;
      priority = 30;
    } else if ((tokens[i].type == TK_EQ || tokens[i].type == TK_NEQ) && priority >= 20) {
      main_operator = i;
      priority = 20;
    }
  }
  assert(main_operator != -1);
  return main_operator;
}

word_t evalExp(int start, int end) {
  if (start > end) {
    return 0;
  } else if (start == end) {
    word_t val;
    if (tokens[start].type == TK_DEC) {
      sscanf(tokens[start].str, "%u", &val);
    } else if (tokens[start].type == TK_HEX) {
      sscanf(tokens[start].str + 2, "%x", &val);
    } else if (tokens[start].type == TK_REG) {
      bool success = false;
      val = isa_reg_str2val(tokens[start].str, &success);
      assert(success);
    }
    return val;
  } else if (checkParentheses(start, end) == true) {
    return evalExp(start + 1, end - 1);
  } else {
    int main_operator = findMainOp(start, end);
    word_t val1 = evalExp(start, main_operator - 1);
    word_t val2 = evalExp(main_operator + 1, end);

    switch (tokens[main_operator].type) {
      case '+':
        return val1 + val2;
      case '-':
        return val1 - val2;
      case '*':
        return val1 * val2;
      case '/': 
        assert(val2 != 0);
        return val1 / val2;
      case TK_AND:
        return val1 && val2;
      case TK_EQ:
        return val1 == val2;
      case TK_NEQ:
        return val1 != val2;
      default:
        assert(false);
    }
  }
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  for (int i = 0; i < nr_token; i ++) {
    if (tokens[i].type == '*' && (i == 0 || 
        (tokens[i-1].type != TK_DEC && tokens[i-1].type != TK_HEX && tokens[i-1].type != TK_REG) )) {
      tokens[i].type = TK_DEREF;
    }
  }

  *success = true;
  return evalExp(0, nr_token - 1);
}
