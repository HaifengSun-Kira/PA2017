#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

#define OPERNUM 4
enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"[0-9]+", TK_NUM},   // decimal number
  {"\\+", '+'},         // plus
  {"\\*", '*'},         // mul
  {"-", '-'},           // minus
  {"/", '/'},           // div
  {"(", '('},           // left parenthesis
  {")", ')'},           // right parenthesis
  {"==", TK_EQ}         // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

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

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
			case '+':
			case '-':
			case '*':
			case '/':
			case '(':
			case ')':
			case TK_EQ:
				tokens[nr_token++].type = rules[i].token_type;
				break;
			case TK_NOTYPE:
				break;
			case TK_NUM:
				tokens[nr_token].type = TK_NUM;
				if (substr_len >= 32) {
					assert(0);
				}
				strncpy(tokens[nr_token].str, substr_start, substr_len);
				nr_token++;
				break;


          default: TODO();
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

static bool check_parentheses(int p, int q){
	if (tokens[p].type == '(' && tokens[q].type == ')'){
		char stack[32];
		int top = 0;
		for(int i=p; i<=q; i++) {
			if (tokens[i].type == '('){
				stack[top++] = '(';
			} else if (tokens[i].type == ')') {
				if (stack[top-1] == '(') {
					top--;
				} else {
					stack[top++] = '(';
				}
			}
		}	
		if (top == 0) {
			return true;
		} else {
			return false;
		}
	}
	return false;
}

static char operators[]={'+', '-', '*', '/'};

static int get_operator_index(char oper){
	for(int i = 0; i < OPERNUM; i++){
		if (oper == operators[i]){
			return i;
		}
	}
	assert(0);
}

int operators_priority[][OPERNUM] = {
	{1, 1, 0, 0},
	{1, 1, 0, 0},
	{1, 1, 1, 1},
	{1, 1, 1, 1}
};

static int dominant_operator(int p, int q) {
	char dopr = '@';
	int dopr_index = -1;
	bool init_flag = false, parenthethese_flag = false;
	for(int i = p; i <= q; i++) {
		switch (tokens[i].type){
			case TK_NUM:
				break;
			case '+': case '-':
			case '*': case '/':
				if (!parenthethese_flag) {
					if (init_flag) {
						if (operators_priority[get_operator_index(dopr)][get_operator_index(tokens[i].type)]){
							dopr = tokens[i].type;
							dopr_index = i;
						}
					} else {
						dopr = tokens[i].type;
						dopr_index = i;
						init_flag = true;
					}
				}
				break;
			case '(':
				parenthethese_flag = true;
				break;
			case ')':
				parenthethese_flag = false;
				break;

		}
	}
	return dopr_index;
}

uint32_t eval(int p, int q) {
  if (p > q) {
    /* Bad expression */
	printf("Bad Expression!\n");
	assert(0);
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
	return atoi(tokens[p].str);
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }
  else {
    int op = dominant_operator(p, q);
    uint32_t val1 = eval(p, op - 1);
    uint32_t val2 = eval(op + 1, q);

    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      default: assert(0);
    }
  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //for (i = 0; i < nr_token; i ++) {
	//if (tokens[i].type == '*' && (i == 0 || tokens[i - 1].type == certain type) ) {
      //tokens[i].type = DEREF;
    //}
  //}

  return eval(0, nr_token - 1);

}
