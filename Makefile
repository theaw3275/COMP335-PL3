all:	scan

scan:	scan.c
			gcc -ansi scan.c -o scan