# main: main.c
# 	gcc -lcurl main.c

main: main.c util.c
	gcc -o octodashcurses -lcurl main.c util.c -I.
