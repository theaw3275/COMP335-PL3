/*
 * scan.c
 * Thea West, Guy Crickner
 * Description
 *    TODO!!
 */

# include <stdio.h>
# include <ctype.h>
# include <stdlib.h>
# include <string.h>

void emit_token(FILE *fpOut, char *name) {
    fputs(name, fpOut);
    fputc(' ', fpOut);
}

int main(int argc, char **argv) {
    char c;
    int inum = 0;
    float fnum;
    float decPlace = 0.1;
    FILE *fpIn;
    fpIn = fopen(argv[1], "r");
    if (!fpIn) return 1; /* something failed w/opening file */
    FILE *fp;
    fp = fopen("tokens.txt", "w");
    if (!fp) return 1; /* something failed w/opening file */

    c = getc(fpIn);
    /* generalized loop for any lexeme */
    while (c != EOF) {
        if (isalpha(c)) { /* we have an letter: abc...xyzABC...XYZ */
                int cap = 20;
	        int len = 0;
	        char *s = (char *) malloc (cap * sizeof (char));
	        if (!s) return 1; /* something failed w/allocation */ 
                while (isalpha(c)) {
	                if (len + 1 >= cap){ /* allocate more space if needed */
		        	cap = cap + 20;
		        	s = (char *) realloc(s, cap * sizeof (char));
		        }
                        s[len] = c;
                        len++;
                        c = getc(fpIn);
                }
                if (strcmp(s, "program") == 0)
                    emit_token(fp, "PROGRAM");
                else if (strcmp(s, "begin") == 0)
                    emit_token(fp, "BEGIN");
                else if (strcmp(s, "end") == 0)
                    emit_token(fp, "END");
                else if (strcmp(s, "int") == 0)
                    emit_token(fp, "INT-TYPE");
                else if (strcmp(s, "float") == 0)
                    emit_token(fp, "FLOAT-TYPE");
                else if (strcmp(s, "if") == 0)
                    emit_token(fp, "IF");
                else if (strcmp(s, "else") == 0)
                    emit_token(fp, "ELSE");
                else if (strcmp(s, "while") == 0)
                    emit_token(fp, "WHILE");
                else
                    emit_token(fp, "ID");
                free(s);      
        }
        else if (isdigit(c)) { /* we have a digit: 0...9 */
                while (isdigit(c)) {
                        /* we have a number */
                        inum = inum * 10 + (c - 48);
                        c = getc(fpIn);
                }
                if (c == '.') {
                        /* we have a float */
                        fnum = inum;
                        c = getc(fpIn);
                        if (isdigit(c)){
                                while (isdigit(c)) {
                                        fnum = fnum + (c-48) * decPlace;
                                        decPlace /= 10;
                                        c = getc(fpIn);
                                }
                                emit_token(fp, "FLOAT");
                        }
                        else{
                                emit_token(fp, "INT");
                                emit_token(fp, "UNDEF");
                        }
                        
                }
                else
                        emit_token(fp, "INT");
        }
        else if (c == ';'){
                emit_token(fp, "STMT-END");
                c = getc(fpIn);
        }
        else if (c == '&'){
                c = getc(fpIn);
                if (c == '&'){
                        emit_token(fp, "AND");
                        c = getc(fpIn);
                }
                else{
                        emit_token(fp, "UNDEF");
                }
        }
        else if (c == '|'){
                c = getc(fpIn);
                if (c == '|'){
                        emit_token(fp, "OR");
                        c = getc(fpIn);
                }
                else{
                        emit_token(fp, "UNDEF");
                }
        }
        else if (c == '+' || c == '-'){
                emit_token(fp, "ADD-OP");
                c = getc(fpIn);
        }
        else if (c == '*' || c == '/' || c == '%'){
                emit_token(fp, "MULT-OP");
                c = getc(fpIn);
        }
        else if (c == '='){
                emit_token(fp, "EQ-OP");
                c = getc(fpIn);
        }
        else if (c == '!'){
                c = getc(fpIn);
                if (c == '='){
                        emit_token(fp, "EQ-OP");
                        c = getc(fpIn);
                }
                else{
                        emit_token(fp, "UNDEF");
                }
        }
        else if (c == '<' || c == '>'){
                emit_token(fp, "RELATIONAL-OP");
                c = getc(fpIn);
                if (c == '='){
                        c = getc(fpIn);
                }
        }
        else if (c == ':'){
                c = getc(fpIn);
                if (c == '='){
                        emit_token(fp, "ASSIGNMENT-OP");
                        c = getc(fpIn);
                }
                else{
                        emit_token(fp, "UNDEF");
                }
        }
        else if (c == '('){
                emit_token(fp, "OPEN-PAREN");
                c = getc(fpIn);
        }
        else if (c == ')'){
                emit_token(fp, "CLOSE-PAREN");
                c = getc(fpIn);
        }
        else if (c == ','){
                emit_token(fp, "COMMA");
                c = getc(fpIn);
        }
        else if (isspace(c)){
                c = getc(fpIn);
        }
        else { /* undefined character */
                emit_token(fp, "UNDEF");
                c = getc(fpIn);
        }
        }

    return 0;
}
