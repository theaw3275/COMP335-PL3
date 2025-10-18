/*
 * scan.c
 * Thea West, Guy Crickner
 * Description
 *    TODO!!
 */
        
# include <stdio.h>
         
void scanFile(FILE *fp);

int main (int argc, char **argv){
        FILE *fp; /* file pointer */
        
        fp = fopen (argv[1], "r");
        if (fp) { /* check if file opened */
                scanFile(fp);
                fclose (fp); 
        } else
                printf ("ERROR - File not found\n");
}

void scanFile(FILE *fp){
        c = fgetc(fp);
        while (c != '\n') {
               /**/ 
        }
}