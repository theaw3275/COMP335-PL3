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

enum tokenType {
    EOF_TOKEN,
    PROGRAM,
    BEGIN,
    END,
    ID,
    IF,
    ELSE,
    WHILE,
    INT_TYPE,
    FLOAT_TYPE,
    INT_CONST,
    FLOAT_CONST,
    STMT_END,
    OPEN_PAREN,
    CLOSE_PAREN,
    AND,
    OR,
    ADD_OP,
    MULT_OP,
    EQ_OP,
    RELATIONAL_OP,
    ASSIGNMENT_OP,
    COMMA,
    UNDEF = -1
};

struct aToken{
   enum tokenType t; 
   int line;
   int character;
};

struct anError{
   char* message;
   int line;
   int character;
   struct anError* ptr;
};

struct aToken token;        /* global (!) token */

int nextToken(FILE *fp);
void program(FILE *fp);
void compound_stmt(FILE *fp);
int isTypeSpecifier();
void declaration_list(FILE *fp);
void statement_list(FILE *fp);
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
void expression_stmt(FILE *fp);
void expression(FILE *fp);
int checkFor(FILE *fp, enum tokenType t);
void continueToSemicolon(FILE *fp);
struct anError* errorFound(char* message);
void printErrors(FILE *fp);


int main(int argc, char **argv) {
    token.line = 1;
    token.character = -1; /* initialize character position (gets incremented by 1) */
    /* open the input file */
    FILE *fp;
    fp = fopen(argv[1], "r");
    if (!fp){
        printf("Error: %s failed to open\n", argv[1]);
        return 1;
    }
    /* get first token */
    nextToken(fp);
    program(fp);
    printf ("\nCompilation done\n");

    /* close and reopen file for error printing */
    fclose(fp);
    fp = fopen(argv[1], "r");
    if (!fp){
        printf("Error: %s failed to open\n", argv[1]);
        return 1;
    }
    printErrors(fp);
    return 0;
}

/* Get Next Token from File (updates global) */
int nextToken(FILE *fp){
    char c = getc(fp);
    token.character++;
    if (c == EOF){
        token.t = EOF_TOKEN;
        return 1;
    }
    else {
        int putbackQ = 1;
        if (isalpha(c)) { /* we have a letter: abc...xyzABC...XYZ */
            int cap = 20;
            int len = 0;
            char *s = (char *) malloc (cap * sizeof (char));
            if (!s){
                errorFound("Memory allocation failed");
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
                    token.character++;
                }
                if (strcmp(s, "program") == 0)
                    token.t = PROGRAM;
                else if (strcmp(s, "begin") == 0)
                    token.t = BEGIN;
                else if (strcmp(s, "end") == 0)
                    token.t = END;
                else if (strcmp(s, "int") == 0)
                    token.t = INT_TYPE;
                else if (strcmp(s, "float") == 0)
                    token.t = FLOAT_TYPE;
                else if (strcmp(s, "if") == 0)
                    token.t = IF;
                else if (strcmp(s, "else") == 0)
                    token.t = ELSE;
                else if (strcmp(s, "while") == 0)
                    token.t = WHILE;
                else
                    token.t = ID;
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
                token.character++;
            }
            if (c == '.') {
                /* we have a float */
                fnum = inum;
                c = getc(fp);
                token.character++;
                if (isdigit(c)){
                    while (isdigit(c)) {
                        fnum = fnum + (c-48) * decPlace;
                        decPlace /= 10;
                        c = getc(fp);
                        token.character++;
                    }
                    token.t = FLOAT_CONST;
                }
                else{
                    token.t = INT_CONST;
                }
            }
            else {
                token.t = INT_CONST;
            }
        }
        else if (c == ';'){
            token.t = STMT_END;
            c = getc(fp);
            token.character++;
        }
        else if (c == '&'){
            c = getc(fp);
            token.character++;
            if (c == '&'){
                token.t = AND;
                c = getc(fp);
                token.character++;
            }
            else{
                token.t = UNDEF;
            }
        }
        else if (c == '|'){
            c = getc(fp);
            token.character++;
            if (c == '|'){
                token.t = OR;
                c = getc(fp);
                token.character++;
            }
            else{
                token.t = UNDEF;
            }
        }
        else if (c == '+' || c == '-'){
            token.t = ADD_OP;
            c = getc(fp);
            token.character++;
        }
        else if (c == '*' || c == '/' || c == '%'){
            token.t = MULT_OP;
            c = getc(fp);
            token.character++;
        }
        else if (c == '='){
            token.t = EQ_OP;
            c = getc(fp);
            token.character++;
        }
        else if (c == '!'){
            c = getc(fp);
            token.character++;
            if (c == '='){
                token.t = EQ_OP;
                c = getc(fp);
                token.character++;
            }
            else{
                token.t = UNDEF;
            }
        }
        else if (c == '<' || c == '>'){
            token.t = RELATIONAL_OP;
            c = getc(fp);
            token.character++;
            if (c == '='){
                c = getc(fp);
                token.character++;
            }
        }
        else if (c == ':'){
            c = getc(fp);
            token.character++;
            if (c == '='){
                token.t = ASSIGNMENT_OP;
                c = getc(fp);
                token.character++;
            }
            else{
                token.t = UNDEF;
            }
        }
        else if (c == '('){
            token.t = OPEN_PAREN;
            c = getc(fp);
            token.character++;
        }
        else if (c == ')'){
            token.t = CLOSE_PAREN;
            c = getc(fp);
            token.character++;
        }
        else if (c == ','){
            token.t = COMMA;
            c = getc(fp);
            token.character++;
        }
        else if (isspace(c)){
            if (c == '\n'){
                token.line++;
                token.character = -1;
            }
            putbackQ = 0;
            nextToken(fp);
        }
        else { /* undefined character */
            token.t = UNDEF;
            c = getc(fp);
            token.character++;
        }
        if (putbackQ == 1) {
            ungetc(c, fp);

            /*DELETE THIS BEFORE SUBMITTING*/
            switch ((int)token.t){
                case 0: printf("TOKEN: EOF-TOKEN\n"); break;
                case 1: printf("TOKEN: PROGRAM\n"); break;
                case 2: printf("TOKEN: BEGIN\n"); break;
                case 3: printf("TOKEN: END\n"); break;
                case 4: printf("TOKEN: ID\n"); break;
                case 5: printf("TOKEN: IF\n"); break;
                case 6: printf("TOKEN: ELSE\n"); break;
                case 7: printf("TOKEN: WHILE\n"); break;
                case 8: printf("TOKEN: INT-TYPE\n"); break;
                case 9: printf("TOKEN: FLOAT-TYPE\n"); break;
                case 10: printf("TOKEN: INT-CONST\n"); break;
                case 11: printf("TOKEN: FLOAT-CONST\n"); break;
                case 12: printf("TOKEN: STMT-END\n"); break;
                case 13: printf("TOKEN: OPEN-PAREN\n"); break;
                case 14: printf("TOKEN: CLOSE-PAREN\n"); break;
                case 15: printf("TOKEN: AND\n"); break;
                case 16: printf("TOKEN: OR\n"); break;
                case 17: printf("TOKEN: ADD-OP\n"); break;
                case 18: printf("TOKEN: MULT-OP\n"); break;
                case 19: printf("TOKEN: EQ_OP\n"); break;
                case 20: printf("TOKEN: RELATIONAL-OP\n"); break;
                case 21: printf("TOKEN: ASSIGNMENT-OP\n"); break;
                case 22: printf("TOKEN: COMMA\n"); break;
                default: printf("TOKEN: UNDEF\n"); break;
            }
            /*STOP DELETING HERE*/
        }
        return 0;
    }
}

/* Program */
void program(FILE *fp){
    checkFor(fp, PROGRAM);
    checkFor(fp, ID); /* main */
    checkFor(fp, OPEN_PAREN);
    checkFor(fp, CLOSE_PAREN);
    compound_stmt(fp);
    if (token.t != EOF_TOKEN){
        errorFound("ERROR: Statements exist beyond end of program\n");
    }
    /* the end! */
}

/* Compound Statement */
void compound_stmt(FILE *fp){
    checkFor(fp, BEGIN);
    if(isTypeSpecifier()){
        declaration_list(fp);
        if (token.t != END){
            statement_list(fp);
        }
    } else {
        statement_list(fp);
    }
    /* nextToken(fp); */
    if(token.t != END){
        errorFound("Error: END expected\n");
    }
    else{
        nextToken(fp);
    }
}

/* Helper for type_specifier(), compound_stmt()
 * returns 1 (true) if token.t == FLOAT or INT
 * returns 0 otherwise */
int isTypeSpecifier(){
    if(token.t == INT_TYPE || token.t == FLOAT_TYPE){
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
    printf("Entered declaration...\n");
    type_specifier(fp);
    init_dec_list(fp);
    if (token.t != STMT_END){
        errorFound("Error: ; expected\n");
    }
    else{
        nextToken(fp);
    }
}

/* Type Specifier
   may need to edit:
   currently doesn't know specific type, only if FLOAT or INT
*/
void type_specifier(FILE *fp){
    if(!isTypeSpecifier()){
        errorFound("Error: Type specifier expected\n");
        /* ADD MORE (maybe?)!!!! WE CARE WHICH TYPE SPEC */
    }
    nextToken(fp);
}

/* Initialized Declarator List */
void init_dec_list(FILE *fp){
    nextToken(fp);
    while (token.t == COMMA){
        nextToken(fp);
        checkFor(fp, ID);
    }
}

/* Statement List */
void statement_list(FILE *fp){
    while (token.t != END && token.t != EOF_TOKEN){
        statement(fp);
    }
}
/* *********** */
void statement(FILE *fp){
                                         printf("Entered statement...\n");
    /* compound statement */
    if (token.t == BEGIN){
        compound_stmt(fp); /* next built in */
    }
        /* conditional statement */
    else if (token.t == IF){
        printf("Entered conditional...\n");
        conditional_stmt(fp); /* next not built in (but ends with statement call) */
    }
        /* while statement */
    else if (token.t == WHILE){
        printf("Entered while...\n");
        nextToken(fp); /*Ignore the WHILE token.t*/
        while_stmt(fp); /* next not built in (but ends with statement call) */
    }
        /* null statement */
    else if (token.t == STMT_END){
        printf("Entered null...");
        nextToken(fp);
    }
        /* expression statement */
    else if (token.t == ID){
        expression_stmt(fp);
    }
    else{
        errorFound("ERROR- Incorrect token here.\n");
        nextToken(fp);
        while (token.t != END && token.t != BEGIN && token.t != IF && token.t != WHILE && token.t != STMT_END && token.t != ID && token.t != EOF_TOKEN) {
            nextToken(fp);
        }
    }
}

void conditional_stmt(FILE *fp){
    checkFor(fp, IF);
    checkFor(fp, OPEN_PAREN);
    conditional_exp(fp);
    checkFor(fp, CLOSE_PAREN);
    /* handles if there's nothing in the if part */
    if (token.t == ELSE){
        errorFound("Error: Nothing in IF branch\n");
        nextToken(fp);
    }
    else {
        statement(fp);
        if (token.t == ELSE) {
            nextToken(fp);
            statement(fp);
        }
    }
}

void conditional_exp(FILE *fp){
    logical_and_exp(fp);
    while(token.t == OR){
        nextToken(fp);
        logical_and_exp(fp);
    }
}

void logical_and_exp(FILE *fp){
    equality_exp(fp);
    while(token.t == AND){
        nextToken(fp);
        equality_exp(fp);
    }
}

void equality_exp(FILE *fp){
    relational_exp(fp);
    while(token.t == EQ_OP){
        nextToken(fp);
        relational_exp(fp);
    }
}

void relational_exp(FILE *fp){
    additive_exp(fp);
    while(token.t == RELATIONAL_OP){
        nextToken(fp);
        additive_exp(fp);
    }
}

void additive_exp(FILE *fp){
    mult_exp(fp);
    while(token.t == ADD_OP){
        nextToken(fp);
        mult_exp(fp);
    }
}

void mult_exp(FILE *fp){
    unary_exp(fp);
    while(token.t == MULT_OP){
        nextToken(fp);
        unary_exp(fp);
    }
}

void unary_exp(FILE *fp){
    if (token.t == ADD_OP){
        nextToken(fp);
    }
    primary_exp(fp);
}

void primary_exp(FILE *fp){
    if(token.t == ID){
        nextToken(fp);
    } else if(token.t == INT_CONST || token.t == FLOAT_CONST){
        nextToken(fp);
    } else if(token.t == OPEN_PAREN){
        paren_exp(fp); /* ends with nextToken */
    } else {
        errorFound("Error: Variable, Int value, or Float value expected\n");
        nextToken(fp);
    }
}

void paren_exp(FILE *fp){
    checkFor(fp, OPEN_PAREN);
    conditional_exp(fp);
    checkFor(fp, CLOSE_PAREN);
}

void while_stmt(FILE *fp){
    checkFor(fp, OPEN_PAREN);
    conditional_exp(fp);
    checkFor(fp, CLOSE_PAREN);
    statement(fp);
}

void expression_stmt(FILE *fp){
    expression(fp);
    checkFor(fp, STMT_END);
}

void expression(FILE *fp){
                                                            printf("Entered assignment...\n");
    if (token.t != ID){
        errorFound("Error: ID expected\n");
        continueToSemicolon(fp);
    }
    else {
        nextToken(fp);
        if (checkFor(fp, ASSIGNMENT_OP) == 0) {
            unary_exp(fp);
        }
        else{
            continueToSemicolon(fp);
        }
    }
}

int checkFor(FILE *fp, enum tokenType t){
    if (token.t != t) {
        /*DELETE THIS BEFORE SUBMITTING*/
        switch ((int)t){
            case 0: errorFound("Error: EOF-TOKEN expected\n"); break;
            case 1: errorFound("Error: PROGRAM expected\n"); break;
            case 2: errorFound("Error: BEGIN expected\n"); break;
            case 3: errorFound("Error: END expected\n"); break;
            case 4: errorFound("Error: ID expected\n"); break;
            case 5: errorFound("Error: IF expected\n"); break;
            case 6: errorFound("Error: ELSE expected\n"); break;
            case 7: errorFound("Error: WHILE expected\n"); break;
            case 8: errorFound("Error: INT-TYPE expected\n"); break;
            case 9: errorFound("Error: FLOAT-TYPE expected\n"); break;
            case 10: errorFound("Error: INT VALUE expected\n"); break;
            case 11: errorFound("Error: FLOAT VALUE expected\n"); break;
            case 12: errorFound("Error: ; expected\n"); break;
            case 13: errorFound("Error: ( expected\n"); break;
            case 14: errorFound("Error: ) expected\n"); break;
            case 15: errorFound("Error: && expected\n"); break;
            case 16: errorFound("Error: || expected\n"); break;
            case 17: errorFound("Error: + or - expected\n"); break;
            case 18: errorFound("Error: * expected\n"); break;
            case 19: errorFound("Error: = or != expected\n"); break;
            case 20: errorFound("Error: > or < expected\n"); break;
            case 21: errorFound("Error: := expected\n"); break;
            case 22: errorFound("Error: , expected\n"); break;
            default: printf("How did you get this error message!?\n"); break;
        }
        /*STOP DELETING HERE*/
        return 1;
    } else {
        nextToken(fp);
        return 0;
    }
}

void continueToSemicolon(FILE *fp){
    printf("[[[Skipping...\n");
    while (token.t != STMT_END && token.t != EOF_TOKEN) {
        nextToken(fp);
    }
    printf("... stopped skipping]]];\n");
}


struct anError* errorFound(char* message){
    static struct anError* first = NULL;
    static struct anError* last = NULL;

    /* sneaky getFirst(), only give message "000" from printErrors() */
    if (strcmp(message, "000") == 0) {
        return first;
    }

    if (first == NULL) {
        first = malloc(sizeof(struct anError));
        first->message = message;
        first->line = token.line;
        first->character = token.character;
        first->ptr = NULL;
        last = first;
    }
    else {
        struct anError* newError = malloc(sizeof(struct anError));
        newError->message = message;
        newError->line = token.line;
        newError->character = token.character;
        newError->ptr = NULL;
        last->ptr = newError;
        last = newError;
    }
    return first;
}

void printErrors(FILE *fp) {
    struct anError* current = errorFound("000");

    if (current == NULL) {
        printf("No errors found.\n");
        return;
    }
    printf("\n\nErrors found:\n");
    int MAX_LINE_LENGTH = sizeof(char) * 256;
    char buffer[MAX_LINE_LENGTH];
    int lineNumber = 1;
    while (fgets(buffer, MAX_LINE_LENGTH, fp) != NULL) {
        if (current == NULL){
            return;
        }
        if (lineNumber == current->line) {
            printf("%3d: %s", current->line, buffer);
            printf("  ");
            int i;
            for (i = 0; i < current->character; i++) {
                printf(" ");
            }
            printf("^\n");
            printf("%s\n", current->message);
            current = current->ptr;
        }
        lineNumber++;
    }
}
