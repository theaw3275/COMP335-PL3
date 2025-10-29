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
#include<stdlib.h> /* for malloc() function */
#include<string.h> /* for strcmp() function */

char* token;        /* global (!) token */

char *nextToken(FILE *fp);
void program(FILE *fp);
void compound_stmt(FILE *fp);
int isTypeSpecifier();
void declaration_list(FILE *fp);
void statement_list(FILE *fp);
void id (FILE *fp);
void declaration(FILE *fp);
void type_specifier(FILE *fp);
void init_dec_list(FILE *fp);

int main() {
   token = (char *) malloc (20 * sizeof (char));
   /* open the token file */
   FILE *fp;
   fp = fopen("tokens.txt", "r");
   if (!fp){
      printf("Error with scanner: tokens.txt failed to open\n");
      return 1;
   }
   /* get first token */
   token = nextToken(fp);
   printf("TOKEN: %s\n", token);
   program(fp);
   printf ("\nCompilation done\n");

   return 0;
}

char* nextToken(FILE *fp){
   fscanf(fp, "%s", token);
   return token;
}

void program(FILE *fp){
   if(strcmp(token, "PROGRAM") != 0){
      printf("Error: PROGRAM expected\n");
   }
   nextToken(fp);
   id(fp);
   if(strcmp(token, "(") == 0){
      nextToken(fp);
      if(strcmp(token, ")") == 0){
         nextToken(fp);
         compound_stmt(fp);
      } else {
         printf("Error: ) expected\n");
      }
   } else {
      printf("Error: ( expected\n");
   }
   /* the end! */
}

void compound_stmt(FILE *fp){
   if(strcmp(token, "BEGIN") != 0){
      printf("Error: BEGIN expected\n");
   }
   nextToken(fp);
   if(isTypeSpecifier()){
      declaration_list(fp);
      nextToken(fp);
      if (strcmp(token, "END") != 0){
         statement_list(fp);
      }
   } else {
      statement_list(fp);
   }
   nextToken(fp);
   if(strcmp(token, "END") != 0){
      printf("Error: END expected\n");
   }
   nextToken(fp);
}

int isTypeSpecifier(){
   if(strcmp(token, "INT-TYPE") == 0 || strcmp(token, "FLOAT-TYPE") == 0){
      return 1;
   }
   return 0;
}

void declaration_list(FILE *fp){
   printf("In declaration_list\n");
   declaration(fp);
   while(isTypeSpecifier()){
      declaration(fp);
   }
}

void declaration(FILE *fp){
   type_specifier(fp);
   init_dec_list(fp);
   if (strcmp(token, "STMT-END") != 0){
      printf("Error: ; expected\n");
   }
   nextToken(fp);
}

void type_specifier(FILE *fp){
   printf("Type specifier is: %s\n", token);
   if(!isTypeSpecifier()){
      printf("Error: Type specifier expected\n");
      /* ADD MORE (maybe?)!!!! WE CARE WHICH TYPE SPEC */
   }
   nextToken(fp);
}

void init_dec_list(FILE *fp){
   id(fp);
   while (strcmp(token, "COMMA") == 0){
      nextToken(fp);
      id(fp);
   }
}

void statement_list(FILE *fp){
   printf("In statement_list\n");
   nextToken(fp);
}

void id (FILE *fp){
   printf("ID is: %s\n", token);
   if(strcmp(token, "ID") != 0){
      printf("Error: ID expected\n");
   }
   nextToken(fp);
}
