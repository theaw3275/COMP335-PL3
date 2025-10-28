/* THIS IS THE EXAMPLE PARSER */


/*
 * COMP 335, Fall 2025
 * littleParser.c
 *
 * Description: 
 * Implements little syntax analyzer for EBNF:
 * <expr> --> <fact> | {+ <fact>}*
 * <fact> --> <id> | {* <id>}*
 * <id> --> A | B | C
 *
 * Input: 
 * An expression on standard input (keyboard)
 *
 * Output:
 * An error message is produced for any syntax error in the input 
 * expression. A message is displayed when scanning is complete.
 *
 * Note: "exp" has been changed to "expr" because exp is a built-in function.
 */


#include<stdio.h>  /* for I/O */
#include<ctype.h>  /* for isdigit() function */

char token;        /* global (!) token */

void expr();
void fact();
void id();
char nextToken();

int main() {
   token = nextToken();
   expr();
   printf ("\nCompilation done\n");

   return 0;
}

/* this represents the scanner */
char nextToken () {
   token = getc(stdin);
   while (token == ' ')  /* skip over whitespace */
      token = getc(stdin);
   return token;
}

void expr () {   
   fact ();
   while (token == '+') {
      printf ("%c", token);
      token = nextToken();
      fact ();
   }
   if (token != '\n')
      /* What you'll want here is a call to an error function,
       * passing to it an error code and the location (position in
       * the line). At some point (after an input '\n' for example), use
       * this information to produce a good message.
       */
      printf ("\nERROR - incorrect operator %c\n", token);
}

void fact () {
   id ();
   while (token == '*') {
      printf ("%c", token);
      token = nextToken();
      id();
   }
}

void id() {
   printf ("%c", token);
   if (token == 'A')
      token = nextToken ();
   else if (token == 'B')
      token = nextToken ();
   else if (token == 'C')
      token = nextToken ();
   else if (token == '\n')
      printf ("\nERROR - missing ID\n");
   else
      printf ("\nERROR - no matching ID %c\n", token);
}