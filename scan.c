/*
 * scan.c
 * Thea West, Guy Crickner
 * Description
 *    Scans a C minus (.cm) source file and produces a list of tokens.
 */

# include <stdio.h>
# include <ctype.h>
# include <stdlib.h>
# include <string.h>

void emit_token(FILE *fpOut, char *name);

void print_output(char *tokenFile, char *codeFile);

int main(int argc, char **argv) {
        char c;
        FILE *fpIn;
        fpIn = fopen(argv[1], "r");
        if (!fpIn) return 1; /* something failed w/opening file */
        FILE *fp;
        char *outFile = "tokens.txt";
        fp = fopen(outFile, "w");
        if (!fp) return 1; /* something failed w/opening file */

        c = getc(fpIn);
        /* generalized loop for any lexeme */
        while (c != EOF) {
                if (isalpha(c)) { /* we have an letter: abc...xyzABC...XYZ */
                        int cap = 20;
                        int len = 0;
                        char *s = (char *) malloc (cap * sizeof (char));
                        if (!s){
                                printf("Memory allocation failed\n");
                                return 1; /* something failed w/allocation */ 
                        }
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
                        int inum = 0;
                        float fnum;
                        float decPlace = 0.1;
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
                                        emit_token(fp, "FLOAT-CONST");
                                }
                                else{
                                        emit_token(fp, "INT-CONST");
                                        emit_token(fp, "UNDEF");
                                }
                                
                        }
                        else
                                emit_token(fp, "INT-CONST");
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
                else if (c == '\n'){
                        emit_token(fp, "endl");
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
        fclose(fpIn);
        fclose(fp);
        print_output(outFile, argv[1]);
        return 0;
}

void emit_token(FILE *fpOut, char *name) {
        if (strcmp(name, "endl") == 0)
            fputc('\n', fpOut);
        else{
                fputs(name, fpOut);
                fputc(' ', fpOut);
        }
}

void print_output(char *tokenFile, char *codeFile){
        FILE *fpTokens;
        FILE *fpCode;
        fpTokens = fopen(tokenFile, "r");
        fpCode = fopen(codeFile, "r");
        if (fpTokens && fpCode){
                char charCode = fgetc(fpCode);
                char charToken = fgetc(fpTokens);
                while (charCode != EOF && charToken != EOF){
                        printf("%c", charCode);
                        charCode = fgetc(fpCode);
                        while (charCode != '\n' && charCode != EOF){
                                printf("%c", charCode);
                                charCode = fgetc(fpCode);
                        }
                        if (charCode == '\n' || charCode == EOF){
                                printf("\n");
                                charCode = fgetc(fpCode);
                        }
                        printf("%c", charToken);
                        charToken = fgetc(fpTokens);
                        while (charToken != '\n' && charToken != EOF){
                                printf("%c", charToken);
                                charToken = fgetc(fpTokens);
                        }
                        if (charToken == '\n'){
                                printf("\n");
                                charToken = fgetc(fpTokens);
                        }
                        while(charToken == '\n' && charCode == '\n'){
                                charCode = fgetc(fpCode);
                                charToken = fgetc(fpTokens);
                        }
                        printf("\n");
                }
        }
}
