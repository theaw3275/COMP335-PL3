all:	scan parse

scan:	scan.c
			gcc -ansi scan.c -o scan

parse:	parse.c
			gcc -ansi parse.c -o parse