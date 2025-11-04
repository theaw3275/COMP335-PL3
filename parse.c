/*
 * TheGuyCo
 * Thea West and Guy Crickmer
 * COMP 335, Fall 2025
 * parse.c
 *
 * Description:
 * TO DO!!!!!!!!
 *
 * Input:
 * The tokens.txt file produced by scan.c
 *
 * Output:
 * An error message is produced for any syntax error in the input
 * expression. A message is displayed when scanning is complete.
 *
 */


#include<stdio.h>  /* for I/O */
#include<ctype.h>  /* for isdigit() function */
#include<stdlib.h> /* for malloc() function */
#include<string.h> /* for strcmp() function */

char* token;        /* global (!) token */

int nextToken(FILE *fp);
void program(FILE *fp);
void compound_stmt(FILE *fp);
int isTypeSpecifier();
void declaration_list(FILE *fp);
void statement_list(FILE *fp);
void id (FILE *fp);
void declaration(FILE *fp);
void type_specifier(FILE *fp);
void init_dec_list(FILE *fp);
void statement_list(FILE *fp);
void statement(FILE *fp);
void conditional_stmt(FILE *fp);
void conditional_exp(FILE *fp);
void logical_and_exp(FILE *fp);
void equality_exp(FILE *fp);
void relational_exp(FILE *fp);
void additive_exp(FILE *fp);
void mult_exp(FILE *fp);
void unary_exp(FILE *fp);
void primary_exp(FILE *fp);
void paren_exp(FILE *fp);
void while_stmt(FILE *fp);
void null_stmt(FILE *fp);
void expression_stmt(FILE *fp);
void expression(FILE *fp);


int main(int argc, char **argv) {
    token = (char *) malloc (20 * sizeof (char));
    /* open the token file */
    FILE *fp;
    fp = fopen(argv[1], "r");
    if (!fp){
        printf("Error with scanner: tokens.txt failed to open\n");
        return 1;
    }
    /* get first token */
    nextToken(fp);
    program(fp);
    printf ("\nCompilation done\n");

    return 0;
}

/* Get Next Token from File (updates global) */
int nextToken(FILE *fp){
    char c = getc(fp);
    if (c == EOF){
        return 1;
    }
    else {
        int putbackQ = 1;
        if (isalpha(c)) { /* we have a letter: abc...xyzABC...XYZ */
            int cap = 20;
            int len = 0;
            char *s = (char *) malloc (cap * sizeof (char));
            if (!s){
                printf("Memory allocation failed\n");
                free(s);
                return 1; /* something failed w/allocation */
            }
            else {
                while (isalpha(c)) {
                    if (len + 1 >= cap) { /* allocate more space if needed */
                        cap = cap + 20;
                        s = (char *) realloc(s, cap * sizeof(char));
                    }
                    s[len] = c;
                    len++;
                    s[len] = '\0';
                    c = getc(fp);
                }
                if (strcmp(s, "program") == 0)
                    token = "PROGRAM";
                else if (strcmp(s, "begin") == 0)
                    token = "BEGIN";
                else if (strcmp(s, "end") == 0)
                    token = "END";
                else if (strcmp(s, "int") == 0)
                    token = "INT-TYPE";
                else if (strcmp(s, "float") == 0)
                    token = "FLOAT-TYPE";
                else if (strcmp(s, "if") == 0)
                    token = "IF";
                else if (strcmp(s, "else") == 0)
                    token = "ELSE";
                else if (strcmp(s, "while") == 0)
                    token = "WHILE";
                else
                    token = "ID";
                free(s);
            }
        }
        else if (isdigit(c)) { /* we have a digit: 0...9 */
            int inum = 0;
            float fnum;
            float decPlace = 0.1;
            while (isdigit(c)) {
                /* we have a number */
                inum = inum * 10 + (c - 48);
                c = getc(fp);
            }
            if (c == '.') {
                /* we have a float */
                fnum = inum;
                c = getc(fp);
                if (isdigit(c)){
                    while (isdigit(c)) {
                        fnum = fnum + (c-48) * decPlace;
                        decPlace /= 10;
                        c = getc(fp);
                    }
                    token = "FLOAT-CONST";
                }
                else{
                    token = "INT-CONST";
                }
            }
            else {
                token = "INT-CONST";
            }
        }
        else if (c == ';'){
            token = "STMT-END";
            c = getc(fp);
        }
        else if (c == '&'){
            c = getc(fp);
            if (c == '&'){
                token = "AND";
                c = getc(fp);
            }
            else{
                token = "UNDEF";
            }
        }
        else if (c == '|'){
            c = getc(fp);
            if (c == '|'){
                token = "OR";
                c = getc(fp);
            }
            else{
                token = "UNDEF";
            }
        }
        else if (c == '+' || c == '-'){
            token = "ADD-OP";
            c = getc(fp);
        }
        else if (c == '*' || c == '/' || c == '%'){
            token = "MULT-OP";
            c = getc(fp);
        }
        else if (c == '='){
            token = "EQ-OP";
            c = getc(fp);
        }
        else if (c == '!'){
            c = getc(fp);
            if (c == '='){
                token = "EQ-OP";
                c = getc(fp);
            }
            else{
                token = "UNDEF";
            }
        }
        else if (c == '<' || c == '>'){
            token = "RELATIONAL-OP";
            c = getc(fp);
            if (c == '='){
                c = getc(fp);
            }
        }
        else if (c == ':'){
            c = getc(fp);
            if (c == '='){
                token = "ASSIGNMENT-OP";
                c = getc(fp);
            }
            else{
                token = "UNDEF";
            }
        }
        else if (c == '('){
            token = "(";
            c = getc(fp);
        }
        else if (c == ')'){
            token = ")";
            c = getc(fp);
        }
        else if (c == ','){
            token = "COMMA";
            c = getc(fp);
        }
        else if (isspace(c)){
            putbackQ = 0;
            nextToken(fp);
        }
        else { /* undefined character */
            token = "UNDEF";
            c = getc(fp);
        }
        if (putbackQ == 1) {
            ungetc(c, fp);
            printf("TOKEN: %s\n", token);
        }
        return 0;
    }
}

/* Program */
void program(FILE *fp){
    if(strcmp(token, "PROGRAM") != 0){
        printf("Error: PROGRAM expected\n");
    }
    nextToken(fp);
    id(fp);
    nextToken(fp);
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

/* Compound Statement */
void compound_stmt(FILE *fp){
    if(strcmp(token, "BEGIN") != 0){
        printf("Error: BEGIN expected\n");
    }
    nextToken(fp);
    if(isTypeSpecifier()){
        declaration_list(fp);
        if (strcmp(token, "END") != 0){
            statement_list(fp);
        }
    } else {
        statement_list(fp);
    }
    /* nextToken(fp); */
    if(strcmp(token, "END") != 0){
        printf("Error: END expected\n");
    }
    nextToken(fp);
}

/* Helper for type_specifier(), compound_stmt()
 * returns 1 (true) if token == FLOAT or INT
 * returns 0 otherwise */
int isTypeSpecifier(){
    if(strcmp(token, "INT-TYPE") == 0 || strcmp(token, "FLOAT-TYPE") == 0){
        return 1;
    }
    return 0;
}

/* Declaration List*/
void declaration_list(FILE *fp){
    declaration(fp);
    while(isTypeSpecifier()){
        declaration(fp);
    }
}

/* Declaration */
void declaration(FILE *fp){
    type_specifier(fp);
    init_dec_list(fp);
    if (strcmp(token, "STMT-END") != 0){
        printf("Error: ; expected\n");
    }
    nextToken(fp);
}

/* Type Specifier
   may need to edit:
   currently doesn't know specific type, only if FLOAT or INT
*/
void type_specifier(FILE *fp){
    if(!isTypeSpecifier()){
        printf("Error: Type specifier expected\n");
        /* ADD MORE (maybe?)!!!! WE CARE WHICH TYPE SPEC */
    }
    nextToken(fp);
}

/* Initialized Declarator List */
void init_dec_list(FILE *fp){
    id(fp);
    nextToken(fp);
    while (strcmp(token, "COMMA") == 0){
        nextToken(fp);
        id(fp);
        nextToken(fp);
    }
}

/* Statement List */
void statement_list(FILE *fp){
    while (strcmp(token, "END") != 0){
        statement(fp);
    }
}
/* *********** */
void statement(FILE *fp){
    /* compound statement */
    if (strcmp(token, "BEGIN") == 0){
        compound_stmt(fp); /* next built in */
    }
        /* conditional statement */
    else if (strcmp(token, "IF") == 0){
        conditional_stmt(fp); /* next not built in (but ends with statement call) */
    }
        /* while statement */
    else if (strcmp(token, "WHILE") == 0){
        while_stmt(fp); /* next not built in (but ends with statement call) */
    }
        /* null statement */
    else if (strcmp(token, "STMT-END") == 0){
        null_stmt(fp);
    }
        /* expression statement */
    else {
        expression_stmt(fp);
    }
}

void conditional_stmt(FILE *fp){
    if(strcmp(token, "IF") != 0){
        printf("Error: IF expected\n");
    }
    nextToken(fp);
    if(strcmp(token, "(") != 0){
        printf("Error: ( expected\n");
    }
    nextToken(fp);
    conditional_exp(fp);
    if(strcmp(token, ")") != 0){
        printf("Error: ) expected\n");
    }
    nextToken(fp);
    /* handles if there's nothing in the if part */
    if (strcmp(token, "ELSE") == 0){
        printf("Warning: Nothing in IF branch\n");
        nextToken(fp);
        statement(fp);
    } else {
        statement(fp);
    }
    if (strcmp(token, "ELSE") == 0){
        nextToken(fp);
        statement(fp);
    }
}

void conditional_exp(FILE *fp){
    logical_and_exp(fp);
    while(strcmp(token, "OR") == 0){
        nextToken(fp);
        logical_and_exp(fp);
    }
}

void logical_and_exp(FILE *fp){
    equality_exp(fp);
    while(strcmp(token, "AND") == 0){
        nextToken(fp);
        equality_exp(fp);
    }
}

void equality_exp(FILE *fp){
    relational_exp(fp);
    while(strcmp(token, "EQ-OP") == 0){
        nextToken(fp);
        relational_exp(fp);
    }
}

void relational_exp(FILE *fp){
    additive_exp(fp);
    while(strcmp(token, "RELATIONAL-OP") == 0){
        nextToken(fp);
        additive_exp(fp);
    }
}

void additive_exp(FILE *fp){
    mult_exp(fp);
    while(strcmp(token, "ADD-OP") == 0){
        nextToken(fp);
        mult_exp(fp);
    }
}

void mult_exp(FILE *fp){
    unary_exp(fp);
    while(strcmp(token, "MULT-OP") == 0){
        nextToken(fp);
        unary_exp(fp);
    }
}

void unary_exp(FILE *fp){
    if (strcmp(token, "ADD-OP") == 0){
        nextToken(fp);
    }
    primary_exp(fp);
}

void primary_exp(FILE *fp){
    if(strcmp(token, "ID") == 0){
        id(fp);
        nextToken(fp);
    } else if(strcmp(token, "INT-CONST") == 0 || strcmp(token, "FLOAT-CONST") == 0){
        nextToken(fp);
    } else if(strcmp(token, "(") == 0){
        paren_exp(fp); /* ends with nextToken */
    } else {
        printf("Error: Primary expression expected\n");
        nextToken(fp);
    }
}

void paren_exp(FILE *fp){
    if(strcmp(token, "(") != 0){
        printf("Error: ( expected\n");
    }
    nextToken(fp);
    conditional_exp(fp); /* TODO: does this end w/nextToken? */
    if(strcmp(token, ")") != 0){
        printf("Error: ) expected\n");
    }
    nextToken(fp);
}

void while_stmt(FILE *fp){
    if(strcmp(token, "WHILE") != 0){
        printf("Error: WHILE expected\n");
    }
    nextToken(fp);
    if(strcmp(token, "(") != 0){
        printf("Error: ( expected\n");
    }
    nextToken(fp);
    conditional_exp(fp);
    if(strcmp(token, ")") != 0){
        printf("Error: ) expected\n");
    }
    nextToken(fp);
    statement(fp);
}

void null_stmt(FILE *fp){
    if(strcmp(token, "STMT-END") != 0){
        printf("Error: ; expected\n");
    }
    nextToken(fp);
}

void expression_stmt(FILE *fp){
    expression(fp);
    if(strcmp(token, "STMT-END") != 0){
        printf("Error: ; expected\n");
    }
    nextToken(fp);
}

void expression(FILE *fp){
    if (strcmp(token, "ID") != 0){
        printf("Error: ID expected\n");
    }
    else {
        nextToken(fp);
        if (strcmp(token, "ASSIGNMENT-OP") != 0) {
            printf("Error: := expected\n");
        }
        nextToken(fp);
        unary_exp(fp);
    }
}

/* ID */
void id (FILE *fp){
    if(strcmp(token, "ID") != 0){
        printf("Error: ID expected\n");
    }
}
